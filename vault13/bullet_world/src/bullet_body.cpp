
#include "bullet_body.h"
#include "irrBulletBoxShape.h"


BulletBody::BulletBody( ISceneNode * p_node )
    : node( p_node ),
      body( 0 ),
      bodyShape( 0 )
{
    if ( node )
        node->grab();
}

BulletBody::~BulletBody()
{
    if ( node )
        node->drop();

    // Don't know how to properly clear.
    /*
    if ( body )
        delete body;
    if ( bodyShape )
        delete bodyShape;
    */
}

bool BulletBody::operator==( const btCollisionShape * shape ) const
{
    if ( shape == bodyShape->getPointer() )
        return true;
    return false;
}

void BulletBody::setShape( const std::string & shape, float mass, IMesh * extra )
{
    ICollisionShape * save = bodyShape;
    bodyShape = 0;
    if ( shape == "box" )
        bodyShape = new IBoxShape( node, mass );
    else if ( shape == "cylinder" )
        bodyShape = new ICylinderShape( node, mass );
    else if ( shape == "sphere" )
        bodyShape = new ISphereShape( node, mass );
    else if ( shape == "bvh_mesh" )
        bodyShape = new IBvhTriangleMeshShape( node, extra, mass );

    if ( !bodyShape )
        bodyShape = save;
}

void BulletBody::setAttribute( const std::string & name, bool value )
{
    if ( body )
        body->getAttributes()->addBool( name.c_str(), value );
}

void BulletBody::applyForce( const core::vector3df & force, const core::vector3df & at, bool local )
{
    if ( body )
    {
        body->setActivationState( EActivationState::EAS_ACTIVE );
        body->applyForce( force, at, local ? ERBTransformSpace::ERBTS_LOCAL : ERBTransformSpace::ERBTS_WORLD );
    }
}

void BulletBody::applyTorque( const core::vector3df & torque, bool local )
{
    if ( body )
    {
        body->setActivationState( EActivationState::EAS_ACTIVE );
        body->applyTorque( torque, local ? ERBTransformSpace::ERBTS_LOCAL : ERBTransformSpace::ERBTS_WORLD );
    }
}

void BulletBody::setSpeed( const core::vector3df & v, bool local )
{
    if ( body )
    {
        body->setActivationState( EActivationState::EAS_ACTIVE );
        body->setLinearVelocity( v, local ? ERBTransformSpace::ERBTS_LOCAL : ERBTransformSpace::ERBTS_WORLD );
    }
}

void BulletBody::setAngVel( const core::vector3df & w, bool local )
{
    if ( body )
    {
        body->setActivationState( EActivationState::EAS_ACTIVE );
        body->setAngularVelocity( w, local ? ERBTransformSpace::ERBTS_LOCAL : ERBTransformSpace::ERBTS_WORLD );
    }
}

void BulletBody::setFriction( float k )
{
    if ( body )
        body->setFriction( k );
}

void BulletBody::setLinearFactor( float x, float y, float z )
{
    if ( body )
        body->setAngularFactor( core::vector3df( x, y, z ) );
}

void BulletBody::setAngularFactor( float x, float y, float z )
{
    if ( body )
        body->setAngularFactor( core::vector3df( x, y, z ) );
}

void BulletBody::position( core::vector3df & at )
{
    if ( body )
        at = body->getLinearVelocity();
}

void BulletBody::orientation( core::quaternion & q )
{
    if ( body )
        q = body->getOrientation();
}

void BulletBody::speed( core::vector3df & v )
{
    if ( body )
        v = body->getLinearVelocity();
}

void BulletBody::angVel( core::vector3df & w )
{
    if ( body )
        w = body->getAngularVelocity();
}





