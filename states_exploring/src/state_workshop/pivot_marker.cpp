
#include "pivot_marker.h"
#include "state_manager.h"

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





void DesignBlock::setMarkersVisible( bool en )
{

}

bool DesignBlock::connectToNearest( std::vector<DesignBlock *> & designs, Ogre::Real & R )
{

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


}



