
#include "entity_planet.h"
#include "entity_world.h"
#include "state_manager.h"
#include "assembly.h"

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

    parent    = 0;
    rotPeriod = 60.0;
    rotTime   = 0.0;

    orbitRadius = 100.0;
    orbitPeriod = 180.0;
    orbitTime   = 0.0;
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

        destroySceneNode();
    }
}

void EntityPlanet::create()
{

}

void EntityPlanet::update()
{

}

Ogre::Real EntityPlanet::gForce( const Assembly & a )
{
    const bool local = (a.parent == this);
    if ( local )
    {
        g.r0 = btVector3( 0.0, 0.0, 0.0 );
        const Ogre::Vector3 r  = a.relR();
        btVector3 f;
        g.gravity( 1.0, btVector3( r.x, r.y, r.z ), f );
        Ogre::Real acc = (Ogre::Real)f.length();
        return acc;
    }

    const Ogre::Vector3 r0 = absoluteR();
    const Ogre::Vector3 r  = a.absoluteR();
    g.r0 = btVector3( r0.x, r0.y, r0.z );
    btVector3 f;
    g.gravity( 1.0, btVector3( r.x, r.y, r.z ), f );
    Ogre::Real acc = (Ogre::Real)f.length();
    return acc;
}

bool EntityPlanet::nearSurface( const Assembly & a ) const
{
    const bool local = (a.parent == this);
    if ( !local )
        return false;

    const Ogre::Real d = atm.radius + atm.height;
    const Ogre::Real r = a.relR().length();
    const bool near = (r < d);
    return near;
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

void EntityPlanet::setR( const Ogre::Vector3 & at )
{
    /*const btVector3 r0 = btVector3( at.x, at.y, at.z );
    btTransform t = rigidBody->getCenterOfMassTransform();
    t.setOrigin( r0 );
    rigidBody->setCenterOfMassTransform( t );
    atm.r0 = r0;
    g.r0   = r0;*/
    sceneNode->setPosition( at );
}

void EntityPlanet::setQ( const Ogre::Quaternion & q )
{
    /*btTransform t = rigidBody->getCenterOfMassTransform();;
    t.setRotation(btQuaternion( q.x, q.y, q.z, q.w ) );
    rigidBody->setCenterOfMassTransform( t );*/

    sceneNode->setOrientation( q );
}

void EntityPlanet::integrateKinematics( Ogre::Real t_sec )
{
    // Absolute rotation.
    rotTime += t_sec;
    if ( rotTime > rotPeriod )
        rotTime -= rotPeriod;
    const Ogre::Quaternion rotQ = absoluteQ();

    setQ( rotQ );


    // Position relative to parent (but in absolute ref. frame).
    orbitTime += t_sec;
    if ( orbitTime > orbitPeriod )
        orbitTime -= orbitPeriod;
    const Ogre::Vector3 at = absoluteR();
    setR( at );
}

Ogre::Vector3 EntityPlanet::absoluteR() const
{
    const Ogre::Real orbAng_2 = 3.1415926535 * orbitTime / orbitPeriod;
    const Ogre::Real o_co2 = std::cos( orbAng_2 );
    const Ogre::Real o_si2 = std::sin( orbAng_2 );
    const Ogre::Quaternion orbitSelfR( 0.0, orbitRadius, 0.0, 0.0 );
    const Ogre::Quaternion orbitSelfQ( o_co2, 0.0, o_si2, 0.0 );
    const Ogre::Quaternion orbitR = orbitQuat * orbitSelfQ *
                                          orbitSelfR *
                                    orbitSelfQ.Inverse() * orbitQuat.Inverse();
    Ogre::Vector3 at( orbitR.x, orbitR.y, orbitR.z );
    if ( parent )
    {
        const Ogre::Vector3 p_at = parent->absoluteR();
        at = p_at + at;
    }

    return at;
}

Ogre::Vector3 EntityPlanet::absoluteV() const
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

    if ( parent )
    {
        const Ogre::Vector3 v_parent = parent->absoluteV();
        v = v_parent + v;
    }

    return v;
}

Ogre::Vector3 EntityPlanet::absoluteW() const
{
    const Ogre::Real w = 2.0*3.1415926535 / rotPeriod;
    Ogre::Quaternion wq( 0.0, 0.0, w, 0.0 );
    wq = rotQuat * wq * rotQuat.Inverse();
    const Ogre::Vector3 wv( wq.x, wq.y, wq.z );
    return wv;
}

// Velocity in abs. ref. frame caused by rotation.
// "local" tells if "at" is in local ref. frame or in global one.
Ogre::Vector3 EntityPlanet::rotV( const Ogre::Vector3 & at, bool local ) const
{
    const Ogre::Vector3 w = absoluteW();
    Ogre::Vector3 r = at;
    if ( local )
    {
        const Ogre::Quaternion q = absoluteQ();
        Ogre::Quaternion rq( 0.0, r.x, r.y, r.z );
        rq = q * rq * q.Inverse();
        r = Ogre::Vector3( rq.x, rq.y, rq.z );
    }
    const Ogre::Vector3 v = w.crossProduct( r );
    return v;
}

Ogre::Quaternion EntityPlanet::absoluteQ() const
{
    // Absolute rotation.
    const Ogre::Real rotAng_2 = 3.1415926535 * rotTime / rotPeriod;
    const Ogre::Real r_co2 = std::cos( rotAng_2 );
    const Ogre::Real r_si2 = std::sin( rotAng_2 );
    const Ogre::Quaternion rotSelfQ( r_co2, 0.0, r_si2, 0.0 );
    const Ogre::Quaternion rotQ = rotQuat * rotSelfQ;
    return rotQ;
}





}



