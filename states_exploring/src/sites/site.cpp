
#include "site.h"
#include "BtOgrePG.h"
#include "BtOgreGP.h"
#include "BtOgreExtras.h"
#include "entity_world.h"
#include "state_manager.h"

namespace Osp
{

Site::Site()
    : Entity()
{
    mType = TSite;

    visualEntity   = 0;
    planet         = 0;
    rigidBody      = 0;
    collisionShape = 0;
}

Site::~Site()
{
    EntityWorld * w = StateManager::getSingletonPtr()->getWorld();
    if ( rigidBody )
    {
        btMotionState * motionState = rigidBody->getMotionState();
        if ( motionState )
            delete motionState;

        w->phyWorld->removeRigidBody( rigidBody );
        delete rigidBody;
    }
    if ( collisionShape )
        delete collisionShape;
    if ( visualEntity )
    {
        Ogre::SceneManager * smgr = StateManager::getSingletonPtr()->getSceneManager();
        if ( smgr->hasEntity( visualEntity->getName() ) )
            smgr->destroyEntity( visualEntity );
    }
    destroySceneNode();
}



}



