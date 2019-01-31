
#include "entity_world.h"
#include "state_manager.h"

namespace Osp
{

static DynamicsWorld * g_ew = 0;
template<> DynamicsWorld * Ogre::Singleton<DynamicsWorld>::msSingleton = 0;


DynamicsWorld *DynamicsWorld::createWorld()
{
    if ( g_ew )
        return g_ew;

    Ogre::SceneManager * scnMgr = StateManager::getSingletonPtr()->getSceneManager();
    if ( !scnMgr )
        return 0;

    g_ew = new DynamicsWorld( scnMgr );
    return g_ew;
}

void DynamicsWorld::deleteWorld()
{
    if ( g_ew )
    {
        delete g_ew;
        g_ew = 0;
    }
}


DynamicsWorld::DynamicsWorld( Ogre::SceneManager * scnMgr )
{
    mBroadphase = new btAxisSweep3(btVector3(-10000,-10000,-10000), btVector3(10000,10000,10000), 1024);
    mCollisionConfig = new btDefaultCollisionConfiguration();
    mDispatcher = new btCollisionDispatcher(mCollisionConfig);
    mSolver = new btSequentialImpulseConstraintSolver();

    phyWorld = new btDiscreteDynamicsWorld(mDispatcher, mBroadphase, mSolver, mCollisionConfig);
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
        phyWorld->stepSimulation( t_sec, 10 );
}

bool DynamicsWorld::frameStarted( const Ogre::FrameEvent & evt, bool debugDraw )
{
    //Update Bullet world. Don't forget the debugDrawWorld() part!
    phyWorld->stepSimulation( evt.timeSinceLastFrame, 10 );

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
    phyWorld->stepSimulation( dt, 10 );

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

