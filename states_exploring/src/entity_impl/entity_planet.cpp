
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

void EntityPlanet::integrateKinematics( Ogre::Real t_sec )
{
    // Absolute rotation.
    rotTime += t_sec;
    if ( rotTime > rotPeriod )
        rotTime -= rotPeriod;
    const Ogre::Real rotAng_2 = 3.1415926535 * rotTime / rotPeriod;
    const Ogre::Real r_co2 = std::cos( rotAng_2 );
    const Ogre::Real r_si2 = std::sin( rotAng_2 );
    const Ogre::Quaternion rotSelfQ( r_co2, 0.0, r_si2, 0.0 );
    const Ogre::Quaternion rotQ = rotQuat * rotSelfQ;

    setRotation( rotQ );


    // Position relative to parent (but in absolute ref. frame).
    orbitTime += t_sec;
    if ( orbitTime > orbitPeriod )
        orbitTime -= orbitPeriod;
    const Ogre::Real orbAng_2 = 3.1415926535 * orbitTime / orbitPeriod;
    const Ogre::Real o_co2 = std::cos( orbAng_2 );
    const Ogre::Real o_si2 = std::sin( orbAng_2 );
    const Ogre::Quaternion orbitSelfR( 0.0, orbitRadius, 0.0, 0.0 );
    const Ogre::Quaternion orbitSelfQ( o_co2, 0.0, o_si2, 0.0 );
    const Ogre::Quaternion orbitR = orbitQuat * orbitSelfQ *
                                        orbitSelfR *
                                    orbitSelfQ.Inverse() * orbitQuat.Inverse();

    const Ogre::Vector3 p_at = ( parent ) ?
                parent->sceneNode->_getDerivedPosition() : Ogre::Vector3();
    const Ogre::Vector3 at( p_at.x + orbitR.x,
                            p_at.x + orbitR.y,
                            p_at.x + orbitR.z );
    setPosition( at );
}

Ogre::Vector3 EntityPlanet::velocity( bool includingParent ) const
{
    //const Ogre::Vector4 v = p_v;

    // Need to compute velocity with respect to it's parent.
    const Ogre::Real w = 1.0/orbitPeriod;
    const Ogre::Vector3 v_w( 0.0, w, 0.0 );
    const Ogre::Real orbAng_2 = 3.1415926535 * orbitTime / orbitPeriod;
    const Ogre::Real o_co2 = std::cos( orbAng_2 );
    const Ogre::Real o_si2 = std::sin( orbAng_2 );
    const Ogre::Quaternion orbitSelfR( 0.0, orbitRadius, 0.0, 0.0 );
    const Ogre::Quaternion orbitSelfQ( o_co2, 0.0, o_si2, 0.0 );
    const Ogre::Quaternion orbitRQ = orbitSelfQ *
                                     orbitSelfR *
                                     orbitSelfQ.Inverse();
    const Ogre::Vector3 v_r( orbitRQ.x, orbitRQ.y, orbitRQ.z );
    Ogre::Vector3 v = v_w.crossProduct( v_r );
    // Need to apply orbit quaternion to have velocity in the right plane.
    const Ogre::Quaternion q_v( 0.0, v.x, v.y, v.z );
    const Ogre::Quaternion q_v_rotated = orbitQuat * q_v * orbitQuat.Inverse();
    v = Ogre::Vector3( q_v_rotated.x, q_v_rotated.y, q_v_rotated.z );

    if ( includingParent && parent )
    {
        const Ogre::Vector3 v_parent = parent->velocity( includingParent );
        v = v_parent + v;
        return v;
    }

    return v;
}

Ogre::Vector3 EntityPlanet::velocityAt( const Ogre::Vector3 & at ) const
{
    const Ogre::Real w = 1.0/rotPeriod;
    Ogre::Quaternion qw( 0.0, 0.0, w, 0.0 );
    qw = rotQuat * qw * rotQuat.Inverse();
    Ogre::Vector3 vw( qw.x, qw.y, qw.z );
    Ogre::Vector3 v = vw.crossProduct( at );
    return v;
}





}



