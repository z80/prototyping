
#include "dynamics_world.h"
#include "state_manager.h"

namespace Osp
{


DynamicsWorld::DynamicsWorld()
{
    Ogre::SceneManager * scnMgr = StateManager::getSingletonPtr()->getSceneManager();

    mBroadphase      = new btAxisSweep3(btVector3(-10000,-10000,-10000), btVector3(10000,10000,10000), 1024);
    mCollisionConfig = new btDefaultCollisionConfiguration();
    mDispatcher      = new btCollisionDispatcher(mCollisionConfig);
    mSolver          = new btSequentialImpulseConstraintSolver();

    phyWorld = new btDiscreteDynamicsWorld( mDispatcher, mBroadphase,
                                            mSolver,     mCollisionConfig );
    phyWorld->setGravity( btVector3( 0.0, 0.0, 0.0 ) );

    dbgdraw = new BtOgre::DebugDrawer( scnMgr->getRootSceneNode(), phyWorld );
    phyWorld->setDebugDrawer( dbgdraw );
}

DynamicsWorld::~DynamicsWorld()
{
    //Free Bullet stuff.
    delete dbgdraw;
    delete phyWorld;

    delete mSolver;
    delete mDispatcher;
    delete mCollisionConfig;
    delete mBroadphase;
}

void DynamicsWorld::integrationStep( Ogre::Real & t_sec, int timeBoost )
{
    for ( int i=0; i<timeBoost; i++ )
        phyWorld->stepSimulation( t_sec, 1 );
}

bool DynamicsWorld::frameStarted( const Ogre::FrameEvent & evt, bool debugDraw )
{
    //Update Bullet world. Don't forget the debugDrawWorld() part!
    phyWorld->stepSimulation( evt.timeSinceLastFrame, 1 );

    if ( debugDraw )
    {
        //phyWorld->debugDrawWorld();

        //Shows debug if F3 key down.
        dbgdraw->step();
    }

    return true;
}

bool DynamicsWorld::frameStarted( Ogre::Real dt, bool debugDraw )
{
    //Update Bullet world. Don't forget the debugDrawWorld() part!
    phyWorld->stepSimulation( dt, 1 );

    if ( debugDraw )
    {
        //phyWorld->debugDrawWorld();

        //Shows debug if F3 key down.
        dbgdraw->step();
    }

    return true;
}

void DynamicsWorld::addEntity( Block * part )
{
    phyWorld->addRigidBody( part->rigidBody );
}

void DynamicsWorld::addEntity( EntityPlanet * part )
{
    phyWorld->addRigidBody( part->rigidBody );
}

void DynamicsWorld::removeEntity( Block * part )
{
    phyWorld->removeRigidBody( part->rigidBody );
}

void DynamicsWorld::removeEntity( EntityPlanet * part )
{
    phyWorld->removeRigidBody( part->rigidBody );
}







}

