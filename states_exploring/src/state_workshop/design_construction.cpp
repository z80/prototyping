
#include "design_construction.h"
#include "tech_tree_panel.h"
#include "state_manager.h"
#include "site_manager_base.h"
#include "site.h"
#include "part_manager_base.h"
#include "pivot_marker.h"

#include <iostream>
#include <sstream>

namespace Osp
{

DesignConstruction::DesignConstruction()
{
    mouseRaySelection  =  false;
    workshop           =  0;
    selectedBlockIndex = -1;
    snapDist           =  0.3;
    rotMouseGain       =  0.001;
    moveMode           =  TFree;

    techTreePanel = new TechTreePanel();
    hintOnNone();
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
    hintOnDrag();
}

bool DesignConstruction::isHovered() const
{
    bool res = techTreePanel->isHovered();
    return res;
}

void DesignConstruction::cameraPlane( Ogre::Vector3 & x, Ogre::Vector3 & y, Ogre::Vector3 & n )
{
    Ogre::Camera * c = StateManager::getSingletonPtr()->getCamera();
    Ogre::Vector3 a( 0.0, 0.0, -1.0 );
    const Ogre::Quaternion q = c->getParentSceneNode()->getOrientation();
    a = q * a;
    // Check if "y" abs value is > 0.707 or not.
    const Ogre::Real TH = 0.707;
    const bool vert = ( std::abs( a.y ) <= TH );
    if ( vert )
    {
        x = Ogre::Vector3( 1.0, 0.0, 0.0 );
        x = q * x;

        y = Ogre::Vector3( 0.0, 1.0, 0.0 );
        n = x.crossProduct( y );
        return;
    }
    x = Ogre::Vector3( 1.0, 0.0, 0.0 );
    x = q * x;
    x = Ogre::Vector3( x.x, 0.0, x.z );
    x.normalise();

    n =  Ogre::Vector3( 0.0, 1.0, 0.0 );
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



    // Absolute coordinates for the ray.
    Ogre::Vector3 r0 = ray.getOrigin();
    Ogre::Vector3 a  = ray.getDirection();

    /*std::cout << "a_abs: " << a.x << " "
                           << a.y << " "
                           << a.z << "   ";*/

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
    {
        hintOnNone();
        return false;
    }
    if ( e->type() != Entity::TPart )
    {
        hintOnNone();
        return false;
    }

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
            hintOnSelect();
            return true;
        }
    }

    hintOnNone();
    return false;
}

bool DesignConstruction::drag()
{
    if ( techTreePanel->isHovered() )
        return false;
    if ( selectedBlockIndex < 0 )
        return false;

    Block * p = blocks[selectedBlockIndex].block;
    const Ogre::Vector3 origin = p->relR();

    Ogre::Vector3 dest;
    mouseAbs( dest, origin );

    std::cout << "dest: " << dest.x << " "
                          << dest.y << " "
                          << dest.z << std::endl;

    //dest = Ogre::Vector3( 0.0, 3.0, 0.0 );
    p->setR( dest );

    return true;
}

void DesignConstruction::dragStart()
{
    hintOnDrag();
    setPivotsVisible( true );
}

void DesignConstruction::dragStop()
{
    StateManager::getSingletonPtr()->setMouseVisible( true );
    setPivotsVisible( false );
    snapToClosestPoint();
}

bool DesignConstruction::rotate()
{
    if ( techTreePanel->isHovered() )
        return false;
    if ( selectedBlockIndex < 0 )
        return false;

    Block * p = blocks[selectedBlockIndex].block;

    Ogre::Vector3 x, y, n;
    cameraPlane( x, y, n );

    int mx, my;
    StateManager::getSingletonPtr()->mouseScreenPos( mx, my );
    Ogre::Vector2 xy = Ogre::Vector2( (Ogre::Real)mx, (Ogre::Real)my );

    const Ogre::Real dy = (xy.y - xyStart.y)*rotMouseGain * 0.5;
    const Ogre::Real co2 = std::cos(dy);
    const Ogre::Real si2 = std::sin(dy);
    const Ogre::Quaternion dq( co2, si2*n.x, si2*n.y, si2*n.z );
    const Ogre::Quaternion q = rotQuatStart * dq;

    p->setQ( q );

    // Rotate around "n".
    return true;
}

void DesignConstruction::rotateStart()
{
    if ( techTreePanel->isHovered() )
        return;
    if ( selectedBlockIndex < 0 )
        return;

    Block * p = blocks[selectedBlockIndex].block;

    int x, y;
    StateManager::getSingletonPtr()->mouseScreenPos( x, y );
    xyStart = Ogre::Vector2( (Ogre::Real)x, (Ogre::Real)y );

    rotQuatStart = p->relQ();

    //StateManager::getSingletonPtr()->setMouseVisible( false );
    hintOnRotate();
    setPivotsVisible( true );
}

void DesignConstruction::rotateStop()
{
    StateManager::getSingletonPtr()->setMouseVisible( true );
    setPivotsVisible( false );
    snapToClosestPoint();
}

void DesignConstruction::destroy()
{
    if ( techTreePanel->isHovered() )
        return;
    if ( selectedBlockIndex < 0 )
        return;

    DesignBlock & db = blocks[selectedBlockIndex];
    db.destroy();

    const int lastInd = (int)(blocks.size()) - 1;
    if ( selectedBlockIndex < lastInd )
        blocks[selectedBlockIndex] = blocks[lastInd];
    blocks.resize(lastInd);

    selectedBlockIndex = -1;
    hintOnNone();
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

bool DesignConstruction::snapToClosestPoint()
{
    if ( selectedBlockIndex < 0 )
        return false;

    DesignBlock & db = blocks[selectedBlockIndex];
    const bool res = db.connectToNearest( blocks, snapDist );

    return res;
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

void DesignConstruction::hintOnNone()
{
    techTreePanel->setTooltip( "Create new blocks using left panel.\n"
                               "Click LMB to select a block.\n"
                               "Press wheel(+shift) to rotate/translate a camera." );
}

void DesignConstruction::hintOnSelect()
{
    if ( selectedBlockIndex < 0 )
        return;

    Block * p = blocks[selectedBlockIndex].block;
    const Ogre::Vector3 origin = Ogre::Vector3::ZERO; //p->relR();

    const std::string name = p->name;
    std::ostringstream out;
    out << "\"" << name << "\" is selected.\n" << "Press \"g\" to grab, "
           " \"r\" to rotate selected a block. \"x\" - to delete it.";
    techTreePanel->setTooltip( out.str() );
}

void DesignConstruction::hintOnDrag()
{
    if ( selectedBlockIndex < 0 )
        return;

    Block * p = blocks[selectedBlockIndex].block;
    const Ogre::Vector3 origin = Ogre::Vector3::ZERO; //p->relR();

    const std::string name = p->name;
    std::ostringstream out;
    out << "Dragging \"" << name << "\". Press LMB to finish.";
    techTreePanel->setTooltip( out.str() );
}

void DesignConstruction::hintOnRotate()
{
    if ( selectedBlockIndex < 0 )
        return;

    Block * p = blocks[selectedBlockIndex].block;
    const Ogre::Vector3 origin = Ogre::Vector3::ZERO; //p->relR();

    const std::string name = p->name;
    std::ostringstream out;
    out << "Rotating \"" << name << "\". Press LMB to finish.";
    techTreePanel->setTooltip( out.str() );
}




}


