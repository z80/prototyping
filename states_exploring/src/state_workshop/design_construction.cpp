
#include "design_construction.h"
#include "tech_tree_panel.h"
#include "state_manager.h"
#include "site_manager_base.h"
#include "site.h"
#include "part_manager_base.h"
#include "pivot_marker.h"

#include <iostream>

namespace Osp
{

DesignConstruction::DesignConstruction()
{
    mouseRaySelection     = false;
    workshop          = 0;
    selectedBlockIndex = -1;
    snapDist          = 10.3;
    moveMode          = TFree;

    techTreePanel = new TechTreePanel();
}

DesignConstruction::~DesignConstruction()
{
    cleanup();

    if ( techTreePanel )
        delete techTreePanel;
}

/// Enter/Leave workshop.
void DesignConstruction::enter()
{
    SiteManagerBase * sm = StateManager::getSingletonPtr()->getSiteManager();
    if ( !sm )
        return;

    Site * s = sm->current( "assembly" );
    workshop = s;

    techTreePanel->updateAllowedParts();

    Ogre::Camera * c = StateManager::getSingletonPtr()->getCamera();
    CameraCtrl * cc = StateManager::getSingletonPtr()->getCameraCtrl();
    //cc->setCamera( 0 );
    cc->setTargetNode( s->sceneNode );
    cc->orbitDist = 10.0;

    Ogre::SceneManager * smgr = StateManager::getSingletonPtr()->getSceneManager();
    smgr->setAmbientLight( Ogre::ColourValue() );
}

void DesignConstruction::leave()
{
    //Ogre::Camera * c = StateManager::getSingletonPtr()->getCamera();
    //CameraCtrl * cc = StateManager::getSingletonPtr()->getCameraCtrl();
    //cc->setCamera( c );
}

/// TechTreePanel callback implementation.
void DesignConstruction::block( const Ogre::String & name )
{
    StateManager * sm = StateManager::getSingletonPtr();
    TechTree * tt = sm->getTechTree();
    PartManagerBase * pm = sm->getPartsManager();
    Block * block = pm->create( name );
    const PartDesc & pd = tt->partDesc( name );

    DesignBlock db;
    db.block = block;
    const size_t connsQty = pd.connections.size();
    for ( size_t i=0; i<connsQty; i++ )
    {
        const ConnectionDesc & cd = pd.connections[i];
        PivotMarker * pivotMarker = new PivotMarker( snapDist );
        pivotMarker->desc = cd;
        db.markers.push_back( pivotMarker );
        pivotMarker->setSceneParent( block );
        pivotMarker->sceneNode->setPosition( cd.r );
    }

    block->setSceneParent( workshop );
    blocks.push_back( db );
    selectedBlockIndex = static_cast<int>( blocks.size() ) - 1;

    // Specify position and rotation.
    // Rotation is easy.
    block->setQ( Ogre::Quaternion::IDENTITY );
    // Position is a point on a plane.
    // And plane is defined by current camera axis vector.
    Ogre::Vector3 xyz;
    mouseAbs( xyz );
    block->setR( xyz );

    // By default drag mode.
    moveMode = TDrag;

    setPivotsVisible( true );
}

bool DesignConstruction::isHovered() const
{
    bool res = techTreePanel->isHovered();
    return res;
}

void DesignConstruction::cameraPlane( Ogre::Vector3 & x, Ogre::Vector3 & y, Ogre::Vector3 & n )
{
    Ogre::Camera * c = StateManager::getSingletonPtr()->getCamera();
    Ogre::Quaternion qa( 0.0, 0.0, 0.0, -1.0 );
    const Ogre::Quaternion q = c->getParentSceneNode()->getOrientation();
    qa = q * qa * q.Inverse();
    // Check if "y" abs value is > 0.707 or not.
    const Ogre::Real TH = 0.707;
    const bool vert = ( std::abs( qa.y ) <= TH );
    if ( vert )
    {
        Ogre::Quaternion qx( 0.0, 1.0, 0.0, 0.0 );
        qx = q * qx * q.Inverse();
        x = Ogre::Vector3( qx.x, 0.0, qx.z );
        x.normalise();

        y = Ogre::Vector3( 0.0, 1.0, 0.0 );

        n = x.crossProduct( y );
        return;
    }
    Ogre::Quaternion qx( 0.0, 1.0, 0.0, 0.0 );
    qx = q * qx * q.Inverse();
    x = Ogre::Vector3( qx.x, 0.0, qx.z );
    x.normalise();

    n =  Ogre::Vector3( 0.0, 0.0, 1.0 );
    y = n.crossProduct( x );
}

void DesignConstruction::mouseInPlane( Ogre::Vector2 & xy )
{

}

void DesignConstruction::mouseAbs( Ogre::Vector3 & xyz, const Ogre::Vector3 & origin )
{
    StateManager * sm = StateManager::getSingletonPtr();
    Ogre::Ray ray;
    const bool rayOk = sm->mouseRay( ray );
    if ( !rayOk )
    {
        xyz = Ogre::Vector3( 0.0, 0.0, 0.0 );
        return;
    }

        Ogre::Camera * cam = sm->getCamera();
        cam->getParentSceneNode()->needUpdate( true );
        const Ogre::Quaternion camQ = cam->getParentSceneNode()->_getDerivedOrientation();
        const Ogre::Vector3    camRelR = cam->getParentSceneNode()->getPosition();
        const Ogre::Vector3    camParR = cam->getParentSceneNode()->getParentSceneNode()->_getDerivedPosition();
        const Ogre::Vector3    camR = cam->getParentSceneNode()->_getDerivedPosition();


    // Absolute coordinates for the ray.
    Ogre::Vector3 r0 = ray.getOrigin();
    Ogre::Vector3 a  = ray.getDirection();

    /*std::cout << "a_abs: " << a.x << " "
                           << a.y << " "
                           << a.z << "   ";*/

    // Need to convery those to relative to assembly node.
    Ogre::SceneNode * camParent   = cam->getParentSceneNode()->getParentSceneNode();
    Ogre::SceneNode * wsSceneNode = workshop->sceneNode;

    const Ogre::Vector3    wsR = workshop->sceneNode->_getDerivedPosition();
    const Ogre::Quaternion wsQ = workshop->sceneNode->_getDerivedOrientation();
    r0 = r0 - wsR;
    Ogre::Quaternion r0q( 0.0, r0.x, r0.y, r0.z );
    r0q = wsQ.Inverse() * r0q * wsQ;
    r0 = Ogre::Vector3( r0q.x, r0q.y, r0q.z );

    Ogre::Quaternion aq( 0.0, a.x, a.y, a.z );
    aq = wsQ.Inverse() * aq * wsQ;
    a = Ogre::Vector3( aq.x, aq.y, aq.z );

    /*std::cout << "a: " << a.x << " "
                       << a.y << " "
                       << a.z << "   ";*/

    Ogre::Vector3 x, y, n;
    cameraPlane( x, y, n );

    // Now need to intersect a ray with a camera plane.
    // Ray:   r = a*t+r0;
    // Plane: (r-origin,n) = 0;
    // Substitute: (a,n)*t = (origin-r0,n);
    // t = -(r0,n)/(a,n);
    const Ogre::Real t_den = a.dotProduct(n);
    if ( std::abs(t_den) < 0.001 )
        xyz = Ogre::Vector3( 0.0, 0.0, 0.0 );
    const Ogre::Real t = (origin-r0).dotProduct(n) / t_den;
    xyz = a*t+r0;
}

bool DesignConstruction::trySelect( int & index )
{
    // Check if mouse is not hovering anything.
    const bool mouseHovering = techTreePanel->isHovered();
    if ( mouseHovering )
        return false;
    StateManager * sm = StateManager::getSingletonPtr();

    Entity * e;
    const bool entityOnTheWay = sm->mouseQuery( e );
    if ( !entityOnTheWay )
        return false;
    if ( e->type() != Entity::TPart )
        return false;

    Block * p_sel = dynamic_cast<Block *>( e );

    const size_t qty = blocks.size();
    for ( size_t i=0; i<qty; i++ )
    {
        DesignBlock & db = blocks[i];
        Block * p = db.block;
        if ( p == p_sel )
        {
            index = static_cast<int>( i );
            selectedBlockIndex = index;
            return true;
        }
    }

    return false;
}

bool DesignConstruction::drag()
{
    if ( techTreePanel->isHovered() )
        return false;
    if ( selectedBlockIndex < 0 )
        return false;

    Block * p = blocks[selectedBlockIndex].block;
    const Ogre::Vector3 origin = Ogre::Vector3::ZERO; //p->relR();

    Ogre::Vector3 dest;
    mouseAbs( dest, origin );

    std::cout << "dest: " << dest.x << " "
                          << dest.y << " "
                          << dest.z << std::endl;

    //dest = Ogre::Vector3( 0.0, 3.0, 0.0 );
    p->setR( dest );

    return true;
}

void DesignConstruction::setPivotsVisible( bool en )
{
    const size_t qty = blocks.size();
    for ( size_t i=0; i<qty; i++ )
    {
        DesignBlock & db = blocks[i];
        db.setPivotsVisible( en );
    }
}

void DesignConstruction::cleanup()
{
    const size_t qty = blocks.size();
    for ( size_t i=0; i<qty; i++ )
    {
        DesignBlock & db = blocks[i];
        db.destroy();
    }
    blocks.clear();

    selectedBlockIndex = -1;
    moveMode = TFree;
}



}


