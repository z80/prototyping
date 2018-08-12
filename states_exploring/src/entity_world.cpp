
#include "entity_world.h"
#include "state_manager.h"

namespace Entity
{

static EntityWorld * g_ew = 0;
template<> EntityWorld * Ogre::Singleton<EntityWorld>::msSingleton = 0;


bool EntityWorld::createWorld()
{
    if ( g_ew )
        return true;

    Ogre::SceneManager * scnMgr = StateManager::getSingletonPtr()->getSceneManager();
    if ( !scnMgr )
        return false;

    g_ew = new EntityWorld( scnMgr );
    return true;
}

void EntityWorld::deleteWorld()
{
    if ( g_ew )
    {
        delete g_ew;
        g_ew = 0;
    }
}


EntityWorld::EntityWorld( Ogre::SceneManager * scnMgr )
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

EntityWorld::~EntityWorld()
{
    //Free Bullet stuff.
    delete dbgdraw;
    delete phyWorld;

    delete mSolver;
    delete mDispatcher;
    delete mCollisionConfig;
    delete mBroadphase;
}

bool EntityWorld::frameStarted( const Ogre::FrameEvent & evt )
{
    //Update Bullet world. Don't forget the debugDrawWorld() part!
    phyWorld->stepSimulation( evt.timeSinceLastFrame, 10 );
    phyWorld->debugDrawWorld();

    //Shows debug if F3 key down.
    dbgdraw->step();
}








}

