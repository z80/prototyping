
#include "entity_factory.h"
#include "entity_world.h"
#include "entity_part.h"
#include "entity_planet.h"
#include "air_mesh.h"
#include "state_manager.h"

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
    //EntityPlanet * spherePlanet();

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

    Ogre::SceneManager * scnMgr = StateManager::getSingletonPtr()->getSceneManager();

    p->visualEntity = scnMgr->createEntity( "cube", "Cube.mesh" );
    p->sceneNode    = scnMgr->getRootSceneNode()->createChildSceneNode( "cubeSceneNode" );

    //Create shape.
    BtOgre::StaticMeshToShapeConverter converter( p->visualEntity );
    p->collisionShape = converter.createSphere();
    p->bodyState      = new BtOgre::RigidBodyState( p->sceneNode );
    p->bodyState->setWorldTransform( btTransform( btQuaternion( 0.0, 0.0, 0.0, 1.0 ),
                                                  btVector3( 0.0, 0.0, 10.0 ) ) );

    //Calculate inertia.
    const btScalar mass = 1.0;
    btVector3      inertia;
    p->collisionShape->calculateLocalInertia( mass, inertia );

    p->rigidBody = new btRigidBody( mass, p->bodyState, p->collisionShape, inertia );

    AirMesh::AirMesh::airMesh( p->visualEntity, p->airMesh );
}

EntityPart   * EntityFactory::PD::plane()
{
    return 0;
}



EntityFactory::EntityFactory()
{
    pd = new PD();
}

EntityFactory::~EntityFactory()
{
    delete pd;
}

Entity * EntityFactory::entityPart( const Ogre::String & name,
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
    return 0;
}






}



