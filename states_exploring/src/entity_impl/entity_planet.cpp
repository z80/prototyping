
#include "entity_planet.h"
#include "entity_world.h"
#include "state_manager.h"

#include <iostream>

namespace Entity
{


EntityPlanet::EntityPlanet()
    : Entity()
{
    visualEntity   = 0;
    sceneNode      = 0;
    rigidBody      = 0;
    collisionShape = 0;
    bodyState      = 0;
}

EntityPlanet::~EntityPlanet()
{
    if ( rigidBody )
    {
        EntityWorld * w = EntityWorld::getSingletonPtr();
        if ( w && w->phyWorld )
            w->phyWorld->removeRigidBody( rigidBody );
    }

    if ( rigidBody )
    {
        btMotionState * motionState = rigidBody->getMotionState();
        if ( motionState )
            delete motionState;

        delete rigidBody;
    }

    if ( collisionShape )
        delete collisionShape;

    Ogre::SceneManager * scnMgr = StateManager::getSingletonPtr()->getSceneManager();
    if ( scnMgr )
    {
        if ( visualEntity );
            scnMgr->destroyEntity( visualEntity );

        if ( sceneNode )
            scnMgr->destroySceneNode( sceneNode );
    }
}

void EntityPlanet::addForces( EntityPart & part )
{
    part.rigidBody->clearForces();
    part.rigidBody->activate( true );

    const btTransform t  = part.rigidBody->getWorldTransform();
    const btVector3 r    = t.getOrigin();
    const btQuaternion q = t.getRotation();
    const btVector3 v = part.rigidBody->getLinearVelocity();
    const btScalar  m = 1.0 / part.rigidBody->getInvMass();

    btVector3 fg;
    g.gravity( m, r, fg );
    btVector3 f( 0.0, 0.0, 0.0 );
    btVector3 p( 0.0, 0.0, 0.0 );
    part.airMesh.forceTorque( atm, r, v, q, f, p );

    //std::cout << "air friction: " << f.x() << ", " << f.y() << ", " << f.z() << std::endl;

    f += fg;
    part.rigidBody->applyCentralForce( f );
    part.rigidBody->applyTorque( p );
}

void EntityPlanet::setPosition( const Ogre::Vector3 & at )
{
    const btVector3 r0 = btVector3( at.x, at.y, at.z );
    btTransform t = rigidBody->getCenterOfMassTransform();
    t.setOrigin( r0 );
    rigidBody->setCenterOfMassTransform( t );
    atm.r0 = r0;
    g.r0   = r0;
}

void EntityPlanet::setRotation( const Ogre::Quaternion & q )
{
    btTransform t = rigidBody->getCenterOfMassTransform();;
    t.setRotation(btQuaternion( q.x, q.y, q.z, q.w ) );
    rigidBody->setCenterOfMassTransform( t );
}






}



