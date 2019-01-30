
#include "site_manager_test.h"
#include "state_manager.h"
#include "planet_manager_base.h"
#include "entity_planet.h"
#include "site.h"

namespace Osp
{

static void createAssembly( SiteManagerTest * mgr );
static void createLaunch( SiteManagerTest * mgr );

SiteManagerTest::SiteManagerTest()
    : SiteManagerBase()
{

}

SiteManagerTest::~SiteManagerTest()
{

}

void SiteManagerTest::createImpl()
{
    createAssembly( this );
}





static void createAssembly( SiteManagerTest * mgr )
{
    PlanetManagerBase * pm = StateManager::getSingletonPtr()->getPlanetsManager();

    Site * s = new Site();
    s->function = "assembly";
    s->name     = "Main assembly";
    s->planet   = pm->planet( "earth" );

    const Ogre::Quaternion q( Ogre::Radian(60.0/180.0*3.1415),
                              Ogre::Vector3( 1.0, 0.0, 0.0 ) );
    Ogre::Quaternion rq( 0.0, 0.0, 1.0, 0.0 );
    rq = q * rq * q.Inverse();
    const Ogre::Vector3 r( rq.x, rq.y, rq.z );

    // Reparent to the appropriate planet.
    s->setSceneParent( s->planet );
    s->setR( r * (30.0 + 1.0) );
    s->setQ( q );

    Ogre::SceneManager * scnMgr = StateManager::getSingletonPtr()->getSceneManager();

    //Ogre::MeshPtr meshPtr = Ogre::MeshManager::getSingletonPtr()->load( "MyPod.mesh", "General" );
    s->visualEntity = scnMgr->createEntity( NameGenerator::Next("pod"), "Pod.mesh" );
    Ogre::UserObjectBindings & uob = s->visualEntity->getUserObjectBindings();
    uob.setUserAny( Ogre::Any( dynamic_cast<Entity *>( s ) ) );
    Ogre::MaterialPtr m = Ogre::MaterialManager::getSingletonPtr()->getByName( "M_Pod" );
    //Ogre::MaterialPtr m = Ogre::MaterialManager::getSingletonPtr()->getDefaultMaterial();
    s->visualEntity->setMaterial( m );
    s->sceneNode    = scnMgr->getRootSceneNode()->createChildSceneNode( NameGenerator::Next("podSceneNode") );
    s->sceneNode->attachObject( s->visualEntity );

    BtOgre::StaticMeshToShapeConverter converter( s->visualEntity );
    s->collisionShape = converter.createCylinder();
    btMotionState * bodyState      = new BtOgre::RigidBodyState( s->sceneNode );
    bodyState->setWorldTransform( btTransform( btQuaternion( q.x, q.y, q.z, q.w ),
                                               btVector3( r.x, r.y, r.z ) ) );

    //Calculate inertia.
    const btScalar mass = 0.0;
    btVector3      inertia( 0.0, 0.0, 0.0 );

    s->rigidBody = new btRigidBody( mass, bodyState, s->collisionShape, inertia );

    mgr->sites.push_back( s );
    mgr->setCurrent( "assembly", "Main assembly" );
}

static void createLaunch( SiteManagerTest * mgr )
{
    PlanetManagerBase * pm = StateManager::getSingletonPtr()->getPlanetsManager();

    Site * s = new Site();
    s->function = "assembly";
    s->name     = "Main assembly";
    s->planet   = pm->planet( "earth" );

    const Ogre::Quaternion q( Ogre::Radian(30.0/180.0*3.1415),
                              Ogre::Vector3( 1.0, 0.0, 0.0 ) );
    Ogre::Quaternion rq( 0.0, 0.0, 1.0, 0.0 );
    rq = q * rq * q.Inverse();
    const Ogre::Vector3 r( rq.x, rq.y, rq.z );

    // Reparent to the appropriate planet.
    s->setSceneParent( s->planet );
    s->setR( r * (30.0 + 1.0) );
    s->setQ( q );

    Ogre::SceneManager * scnMgr = StateManager::getSingletonPtr()->getSceneManager();

    //Ogre::MeshPtr meshPtr = Ogre::MeshManager::getSingletonPtr()->load( "MyPod.mesh", "General" );
    s->visualEntity = scnMgr->createEntity( NameGenerator::Next("pod"), "Pod.mesh" );
    Ogre::UserObjectBindings & uob = s->visualEntity->getUserObjectBindings();
    uob.setUserAny( Ogre::Any( dynamic_cast<Entity *>( s ) ) );
    Ogre::MaterialPtr m = Ogre::MaterialManager::getSingletonPtr()->getByName( "M_Pod" );
    //Ogre::MaterialPtr m = Ogre::MaterialManager::getSingletonPtr()->getDefaultMaterial();
    s->visualEntity->setMaterial( m );
    s->sceneNode    = scnMgr->getRootSceneNode()->createChildSceneNode( NameGenerator::Next("podSceneNode") );
    s->sceneNode->attachObject( s->visualEntity );

    BtOgre::StaticMeshToShapeConverter converter( s->visualEntity );
    s->collisionShape = converter.createCylinder();
    btMotionState * bodyState      = new BtOgre::RigidBodyState( s->sceneNode );
    bodyState->setWorldTransform( btTransform( btQuaternion( q.x, q.y, q.z, q.w ),
                                               btVector3( r.x, r.y, r.z ) ) );

    //Calculate inertia.
    const btScalar mass = 0.0;
    btVector3      inertia( 0.0, 0.0, 0.0 );

    s->rigidBody = new btRigidBody( mass, bodyState, s->collisionShape, inertia );

    mgr->sites.push_back( s );
    mgr->setCurrent( "launch", "Main launch" );
}

}




