
#include "entity_factory.h"
#include "entity_world.h"
#include "entity_part.h"
#include "entity_planet.h"
#include "air_mesh.h"
#include "state_manager.h"
#include "name_generator.h"

namespace Entity
{

static EntityFactory g_ef;
template<> EntityFactory * Ogre::Singleton<EntityFactory>::msSingleton = 0;

class EntityFactory::PD
{
public:
    EntityWorld  * world();
    EntityPart   * cube();
    EntityPart   * plane();
    EntityPlanet * spherePlanet();

    PD();
    ~PD();
};

EntityFactory::PD::PD()
{

}

EntityFactory::PD::~PD()
{

}

EntityWorld  * EntityFactory::PD::world()
{
    EntityWorld * w = EntityWorld::createWorld();
    return w;
}

EntityPart   * EntityFactory::PD::cube()
{
    EntityPart * p = new EntityPart();

    Ogre::MaterialManager::getSingletonPtr()->getDefaultMaterial();

    Ogre::SceneManager * scnMgr = StateManager::getSingletonPtr()->getSceneManager();

    p->visualEntity = scnMgr->createEntity( NameGenerator::Next("cube"), "Cube.mesh" );
    Ogre::UserObjectBindings & uob = p->visualEntity->getUserObjectBindings();
    uob.setUserAny( Ogre::Any( p ) );
    Ogre::MaterialPtr m = Ogre::MaterialManager::getSingletonPtr()->getByName( "MyGrass" );
    //Ogre::MaterialPtr m = Ogre::MaterialManager::getSingletonPtr()->getDefaultMaterial();
    p->visualEntity->setMaterial( m );
    p->sceneNode    = scnMgr->getRootSceneNode()->createChildSceneNode( NameGenerator::Next("cubeSceneNode") );
    p->sceneNode->attachObject( p->visualEntity );

    //Create shape.
    BtOgre::StaticMeshToShapeConverter converter( p->visualEntity );
    p->collisionShape = converter.createBox();
    p->bodyState      = new BtOgre::RigidBodyState( p->sceneNode );
    p->bodyState->setWorldTransform( btTransform( btQuaternion( 0.0, 0.0, 0.0, 1.0 ),
                                                  btVector3( 5.0, 10.0, 110.0 ) ) );

    //Calculate inertia.
    const btScalar mass = 1.0;
    btVector3      inertia;
    p->collisionShape->calculateLocalInertia( mass, inertia );

    p->rigidBody = new btRigidBody( mass, p->bodyState, p->collisionShape, inertia );

    AirMesh::AirMesh::airMesh( p->visualEntity, p->airMesh );

    EntityWorld::getSingletonPtr()->addEntity( p );
    return p;
}

EntityPart   * EntityFactory::PD::plane()
{
    EntityPart * p = new EntityPart();

    Ogre::SceneManager * scnMgr = StateManager::getSingletonPtr()->getSceneManager();

    p->visualEntity = scnMgr->createEntity( NameGenerator::Next("plane"), "Plane.mesh" );
    Ogre::UserObjectBindings & uob = p->visualEntity->getUserObjectBindings();
    uob.setUserAny( Ogre::Any( p ) );
    Ogre::MaterialPtr m = Ogre::MaterialManager::getSingletonPtr()->getByName( "MyGrass" );
    //Ogre::MaterialPtr m = Ogre::MaterialManager::getSingletonPtr()->getDefaultMaterial();
    p->visualEntity->setMaterial( m );
    p->sceneNode    = scnMgr->getRootSceneNode()->createChildSceneNode( NameGenerator::Next("planeSceneNode") );
    p->sceneNode->attachObject( p->visualEntity );
    p->sceneNode->setScale( 100.0, 100.0, 100.0 );

    //Create shape.
    BtOgre::StaticMeshToShapeConverter converter( p->visualEntity );
    p->collisionShape = new btStaticPlaneShape( btVector3( 0.0, 0.0, 1.0 ), 0.0 );
    p->bodyState      = new BtOgre::RigidBodyState( p->sceneNode );
    p->bodyState->setWorldTransform( btTransform( btQuaternion( 0.0, 0.0, 0.0, 1.0 ),
                                                  btVector3( 0.0, 0.0, 0.0 ) ) );

    //Calculate inertia.
    const btScalar mass = 0.0;
    btVector3      inertia( 0.0, 0.0, 0.0 );

    p->rigidBody = new btRigidBody( mass, p->bodyState, p->collisionShape, inertia );
    //p->rigidBody->activate( true );

    AirMesh::AirMesh::airMesh( p->visualEntity, p->airMesh );

    EntityWorld::getSingletonPtr()->addEntity( p );

    return p;
}

EntityPlanet * EntityFactory::PD::spherePlanet()
{
    const btScalar R = 100.0;


    EntityPlanet * p = new EntityPlanet();

    Ogre::SceneManager * scnMgr = StateManager::getSingletonPtr()->getSceneManager();

    p->visualEntity = scnMgr->createEntity( NameGenerator::Next("sphere"), "Sphere.mesh" );
    Ogre::UserObjectBindings & uob = p->visualEntity->getUserObjectBindings();
    uob.setUserAny( Ogre::Any( p ) );
    Ogre::MaterialPtr m = Ogre::MaterialManager::getSingletonPtr()->getByName( "Sphere" );
    //Ogre::MaterialPtr m = Ogre::MaterialManager::getSingletonPtr()->getDefaultMaterial();
    p->visualEntity->setMaterial( m );
    p->sceneNode    = scnMgr->getRootSceneNode()->createChildSceneNode( NameGenerator::Next("sphereSceneNode") );
    p->sceneNode->attachObject( p->visualEntity );
    p->sceneNode->setScale( R, R, R );

    //Create shape.
    BtOgre::StaticMeshToShapeConverter converter( p->visualEntity );
    p->collisionShape = converter.createTrimesh();
    p->bodyState      = new BtOgre::RigidBodyState( p->sceneNode );
    p->bodyState->setWorldTransform( btTransform( btQuaternion( 0.0, 0.0, 0.0, 1.0 ),
                                                  btVector3( 0.0, 0.0, 0.0 ) ) );

    //Calculate inertia.
    const btScalar mass = 0.0;
    btVector3      inertia( 0.0, 0.0, 0.0 );

    p->rigidBody = new btRigidBody( mass, p->bodyState, p->collisionShape, inertia );
    //p->rigidBody->activate( true );

    p->atm.radius = R;
    p->g.radius   = R;

    EntityWorld::getSingletonPtr()->addEntity( p );

    return p;

}









EntityFactory::EntityFactory()
{
    pd = new PD();
}

EntityFactory::~EntityFactory()
{
    delete pd;
}

Entity * EntityFactory::create( const std::string & name,
                                const Ogre::Vector3 & at,
                                const Ogre::Quaternion & q,
                                Ogre::SceneNode * parentNode )
{
    if ( name == "world" )
        return pd->world();
    else if ( name == "cube" )
        return pd->cube();
    else if ( name == "plane" )
        return pd->plane();
    else if ( name == "planet" )
        return pd->spherePlanet();
    return 0;
}






}



