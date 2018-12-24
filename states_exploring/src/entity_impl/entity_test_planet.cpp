
#include "entity_test_planet.h"
#include "OgreSceneManager.h"
#include "state_manager.h"

namespace Entity
{

EntityTestPlanet::EntityTestPlanet()
    :EntityPlanet()
{
    R = 100.0;
}

EntityTestPlanet::~EntityTestPlanet()
{

}

void EntityTestPlanet::create()
{
    const btScalar R = this->R;


    EntityPlanet * p = new EntityPlanet();

    Ogre::SceneManager * scnMgr = StateManager::getSingletonPtr()->getSceneManager();

    p->visualEntity = scnMgr->createEntity( NameGenerator::Next("sphere"), "Sphere.mesh" );
    Ogre::UserObjectBindings & uob = p->visualEntity->getUserObjectBindings();
    uob.setUserAny( Ogre::Any( p ) );
    Ogre::MaterialPtr m = Ogre::MaterialManager::getSingletonPtr()->getByName( "MaterialSphere" );
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
    p->atm.height = R*0.1;
    p->g.radius   = R;
    p->g.GM       = 9.81*R*R;
}

void EntityTestPlanet::update()
{
    // Nothing should happen her for static mesh collision shape.
}




}

