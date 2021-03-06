
#include "part_manager_test.h"
#include "entity_part.h"
#include "air_mesh.h"
#include "state_manager.h"
#include "name_generator.h"

namespace Osp
{

class PartManagerTest::PD
{
public:
    Block * basePart( const Ogre::String & meshName, const Ogre::String & materialName );
    Block * cube();
    Block * sphere();

    PD();
    ~PD();
};

PartManagerTest::PD::PD()
{

}

PartManagerTest::PD::~PD()
{

}

Block * PartManagerTest::PD::basePart( const Ogre::String & meshName, const Ogre::String & materialName )
{
    Block * p = new Block();

    Ogre::MaterialManager::getSingletonPtr()->getDefaultMaterial();

    Ogre::SceneManager * scnMgr = StateManager::getSingletonPtr()->getSceneManager();

    p->visualEntity = scnMgr->createEntity( NameGenerator::Next( meshName ), meshName );
    Ogre::UserObjectBindings & uob = p->visualEntity->getUserObjectBindings();
    uob.setUserAny( Ogre::Any( dynamic_cast<Entity *>( p ) ) );
    Ogre::MaterialPtr m = Ogre::MaterialManager::getSingletonPtr()->getByName( materialName );
    p->visualEntity->setMaterial( m );

    p->visualNode = p->sceneNode->createChildSceneNode( NameGenerator::Next( meshName ) );
    p->visualNode->attachObject( p->visualEntity );
    p->visualNode->setScale( 1.0, 1.0, 1.0 );
    p->visualNode->setInheritOrientation( true );
    p->visualNode->setInheritScale( false );
    return p;
}


Block   * PartManagerTest::PD::cube()
{
    Block * p = basePart( "Cube.mesh", "M_Cube" );

    //Create shape.
    BtOgre::StaticMeshToShapeConverter converter( p->visualEntity );
    p->collisionShape = converter.createBox();
    p->bodyState      = new BtOgre::RigidBodyState( p->sceneNode );
    p->bodyState->setWorldTransform( btTransform( btQuaternion( 0.0, 0.0, 0.0, 1.0 ),
                                                  btVector3( 0.0, 0.0, 0.0 ) ) );

    //Calculate inertia.
    const btScalar mass = 1.0;
    btVector3      inertia( 1.0, 1.0, 1.0 );
    p->collisionShape->calculateLocalInertia( mass, inertia );

    p->rigidBody = new btRigidBody( mass, p->bodyState, p->collisionShape, inertia );
    // It is potentially very fast moving body.
    // Because of that Continuous Collision Detection is enabled.
    p->rigidBody->setCcdMotionThreshold( 1.0e-7 );
    p->rigidBody->setCcdSweptSphereRadius( 0.5 );

    AirMesh::AirMesh::airMesh( p->visualEntity, p->airMesh );

    return p;
}


Block * PartManagerTest::PD::sphere()
{
    Block * p = basePart( "Sphere", "MSphere" );

    //Create shape.
    BtOgre::StaticMeshToShapeConverter converter( p->visualEntity );
    p->collisionShape = converter.createSphere();
    p->bodyState      = new BtOgre::RigidBodyState( p->sceneNode );
    p->bodyState->setWorldTransform( btTransform( btQuaternion( 0.0, 0.0, 0.0, 1.0 ),
                                                  btVector3( 0.0, 0.0, 0.0 ) ) );

    //Calculate inertia.
    const btScalar mass = 1.0;
    btVector3      inertia( 1.0, 1.0, 1.0 );

    p->rigidBody = new btRigidBody( mass, p->bodyState, p->collisionShape, inertia );
    // It is potentially very fast moving body.
    // Because of that Continuous Collision Detection is enabled.
    p->rigidBody->setCcdMotionThreshold( 1.0e-7 );
    p->rigidBody->setCcdSweptSphereRadius( 0.5 );
    //p->rigidBody->activate( true );

    AirMesh::AirMesh::airMesh( p->visualEntity, p->airMesh );

    return p;

}









PartManagerTest::PartManagerTest()
{
    pd = new PD();
}

PartManagerTest::~PartManagerTest()
{
    delete pd;
}

Block * PartManagerTest::create( const std::string & name,
                                      const Ogre::Vector3 & r,
                                      const Ogre::Quaternion & q )
{
    Block * p = 0;
    if ( name == "cube" )
        p = pd->cube();
    else if ( name == "planet" )
        p = pd->sphere();

    if ( p )
    {
        p->setR( r );
        p->setQ( q );
    }
    return p;
}






}



