
#include "pivot_marker.h"
#include "state_manager.h"
#include "site.h"

namespace Osp
{

PivotMarker::PivotMarker( Ogre::Real sz )
    : Entity()
{
    Ogre::SceneManager * smgr = StateManager::getSingletonPtr()->getSceneManager();

    const Ogre::String meshName     = "PivotMarker.mesh";
    const Ogre::String materialName = "M_PivotMarker";

    visualEntity = smgr->createEntity( NameGenerator::Next( meshName ), meshName );
    Ogre::UserObjectBindings & uob = visualEntity->getUserObjectBindings();
    uob.setUserAny( Ogre::Any( dynamic_cast<Entity *>( this ) ) );
    Ogre::MaterialPtr m = Ogre::MaterialManager::getSingletonPtr()->getByName( materialName );
    visualEntity->setMaterial( m );

    sceneNode->attachObject( visualEntity );
    sceneNode->setScale( sz, sz, sz );
}

PivotMarker::~PivotMarker()
{
    Ogre::SceneManager * smgr = StateManager::getSingletonPtr()->getSceneManager();
    if ( visualEntity )
        smgr->destroyEntity( visualEntity );
}

void PivotMarker::setSize( Ogre::Real sz )
{
    sceneNode->setScale( sz, sz, sz );
}





bool DesignBlock::connectToNearest( std::vector<DesignBlock *> & designs, Ogre::Real & R )
{
    const size_t localMakersQty = markers.size();
    const size_t blocksQty      = designs.size();
    Ogre::Real minDist = -1.0;
    size_t     bestLocalMarkerInd  = 0;
    size_t     bestBlockIndex      = 0;
    size_t     bestRemoteMarkerInd = 0;
    for ( size_t localInd=0; localInd<localMakersQty; localInd++ )
    {
        for ( size_t blockInd=0; blockInd<blocksQty; blockInd++ )
        {
            PivotMarker * localMarker = this->markers[localInd];
            DesignBlock * block = designs[blockInd];
            if ( block == this )
                continue;

            const size_t markersQty = block->markers.size();
            for ( size_t markerInd=0; markerInd<markersQty; markerInd++ )
            {
                PivotMarker * marker = block->markers[markerInd];
                Ogre::Vector3    r;
                Ogre::Quaternion q;
                const bool relPoseOk = localMarker->relativePose( marker, r, q );
                if ( !relPoseOk )
                    continue;

                const Ogre::Real d = r.length();
                if ( ( minDist < 0.0 ) || (d < minDist) )
                {
                    bestLocalMarkerInd  = localInd;
                    bestBlockInd        = blockInd;
                    bestRemoteMarkerInd = markerInd;
                    minDist             = d;
                }
            }
        }
    }
    if ( minDist < 0.0 )
        return false;

    if ( minDist > 2.0*R )
        return false;

    // Compute what relative position should be.
    // Alost compute relative orientation. Reparent
    // and set position and orientation.
    PivotMarker * localMarker  = this->markers[bestLocalMarkerInd];
    DesignBlock * parentBlock  = designs[bestBlockInd];
    PivotMarker * parentMarker = parentBlock->markers[bestRemoteMarkerInd];
    Ogre::Vector3    r;
    Ogre::Quaternion q;
    const bool relPoseOk = parentMarker->relativePose( localMarker, r, q );
    this->block->setSceneParent( parentBlock, true );
    this->block->setR( r );
    this->block->setQ( q );

    return relPoseOk;
}

bool DesignBlock::disconnectFromParent( Site * site )
{
    Ogre::SceneNode * siteNode = site->sceneNode;
    Ogre::SceneNode * currentParent = this->block->sceneNode->getParentSceneNode();
    if ( siteNode == currentParent )
        return true;

    Ogre::Vector3    r;
    Ogre::Quaternion q;
    const bool relOk = this->block->relativePose( site, r, q );
    if ( !relOk )
        return false;

    this->block->setSceneParent( site, true );
    this->block->setR( r );
    this->block->setQ( q );

    return true;
}

DesignBlock::DesignBlock()
{
    block = 0;
}

DesignBlock::~DesignBlock()
{
    // Don't delete anything here.
    // This class is just for grouping purpose.
}

DesignBlock::DesignBlock( const DesignBlock & inst )
{
    *this = inst;
}

const DesignBlock & DesignBlock::operator=( const DesignBlock & inst )
{
    if ( this != &inst )
    {
        block   = inst.block;
        markers = inst.markers;
    }
    return *this;
}

void DesignBlock::setPivotsVisible( bool en )
{
    const size_t markersQty = markers.size();
    for ( size_t i=0; i<markersQty; i++ )
    {
        PivotMarker * pm = markers[i];
        pm->visualEntity->setVisible( en );;
    }
}

void DesignBlock::destroy()
{
    const size_t markersQty = markers.size();
    for ( size_t i=0; i<markersQty; i++ )
    {
        PivotMarker * pm = markers[i];
        delete pm;
    }
    delete block;
}


}



