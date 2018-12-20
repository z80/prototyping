
#include "btBulletDynamicsCommon.h"
#include "irrBullet.h"
#include "skeleton_body.h"
#include "BulletCollision/CollisionDispatch/btCollisionWorld.h"


SkeletonBody::SkeletonBody( IAnimatedMeshSceneNode * p_node,
                            float bodyMass, float jointMass, float radius )
    : node( p_node )
{
    m_rootShift.setValue( 0.0f, 1.0f, 0.0f );

    if ( p_node )
        p_node->grab();

    ISkinnedMesh * mesh = dynamic_cast<ISkinnedMesh *>( node->getMesh() );
    if ( !mesh )
        return;

    core::matrix4 rootAt = node->getAbsoluteTransformation();

    core::vector3df bias = rootAt.getTranslation();
    bias.X += m_rootShift.x();
    bias.Y += m_rootShift.y();
    bias.Z += m_rootShift.z();
    rootAt.setTranslation( bias );

    core::matrix4 rootInv;
    bool res = rootAt.getInverse( rootInv );
    if ( !res )
        return;


    // Create root rigid body.
    btSphereShape * shape = new btSphereShape( radius );

    btVector3 localInertia;
    shape->calculateLocalInertia( jointMass, localInertia );
    btRigidBody::btRigidBodyConstructionInfo ci( bodyMass, 0, shape, localInertia );
    core::vector3df at = p_node->getAbsolutePosition();
    ci.m_startWorldTransform.setOrigin( btVector3( at.X, at.Y, at.Z ) + m_rootShift );

    m_rootBody = new btRigidBody( ci );
    m_rootBody->setLinearFactor( btVector3( 1, 1, 1 ) );
    m_rootBody->setAngularFactor( btVector3( 0, 0, 0 ) );

    // m_world->addRigidBody( m_rigidBody );
    m_rootBody->setFriction( 10.0f );



    // Parse all bones and add something there.
    core::array<ISkinnedMesh::SJoint*> & joints = mesh->getAllJoints();
    int sz = joints.size();
    for ( int i=0; i<sz; i++ )
    {
        // Create sphere collision shape.
        btSphereShape * shape = new btSphereShape( radius );

        btVector3 localInertia;
        shape->calculateLocalInertia( jointMass, localInertia );
        btRigidBody::btRigidBodyConstructionInfo ci( jointMass, 0, shape, localInertia );

        IBoneSceneNode * jn = p_node->getJointNode( i );
        // Compute relative transformation.
        ISkinnedMesh::SJoint * sj = joints[i];
        core::matrix4 absAt = jn->getAbsoluteTransformation();
        core::matrix4 relAt = rootInv * absAt;
        btTransform tr;
        btTransformFromIrrlichtMatrix( absAt, tr );

        // Initialize rigid body for the joint.
        ci.m_startWorldTransform = tr;

        btRigidBody * body =  new btRigidBody( ci );
        body->setLinearFactor( btVector3( 1, 1, 1 ) );
        body->setAngularFactor( btVector3( 0, 0, 0 ) );

        // m_world->addRigidBody( m_rigidBody );
        body->setFriction( 10.0f );
        m_bodies.push_back( body );

        btTransformFromIrrlichtMatrix( relAt, tr );
        btTransform identity;
        identity.setIdentity();
        // Add fixed constraint.
        btFixedConstraint * c = new btFixedConstraint( *m_rootBody, *body, tr, identity );
        c->setBreakingImpulseThreshold( 10.0 * 100.0 );
        c->setOverrideNumSolverIterations( 120 );
        m_constraints.push_back( c );
    }


    // Make it kinematic object.
    // Dynamic objects can interact with it
    // but can't affect it.
    /*
    m_rigidBody->setCollisionFlags( m_rigidBody->getCollisionFlags() |
                                    btCollisionObject::CF_KINEMATIC_OBJECT );
    m_rigidBody->setActivationState( DISABLE_DEACTIVATION );
    */
}

SkeletonBody::~SkeletonBody()
{

}

void SkeletonBody::addToWorld( irrBulletWorld * world )
{
    world->getPointer()->addRigidBody( m_rootBody );
    int sz = static_cast<int>( m_bodies.size() );
    for ( int i=0; i<sz; i++ )
    {
        btRigidBody * b = m_bodies[i];
        world->getPointer()->addRigidBody( b );
    }
    sz = static_cast<int>( m_constraints.size() );
    for ( int i=0; i<sz; i++ )
    {
        btFixedConstraint * co = m_constraints[i];
        world->getPointer()->addConstraint( co, true );
        //return;
    }
}

void SkeletonBody::updateShape()
{
    ISkinnedMesh * mesh = dynamic_cast<ISkinnedMesh *>( node->getMesh() );
    if ( !mesh )
        return;

    // Applying rigid body position to scene node.
    btTransform trRoot;
    trRoot = m_rootBody->getWorldTransform();
    btVector3 at = trRoot.getOrigin();
    at -= m_rootShift;
    node->setPosition( core::vector3df( at.x(), at.y(), at.z() ) );
    node->updateAbsolutePosition();

    core::matrix4 rootAt = node->getAbsoluteTransformation();

    core::vector3df bias = rootAt.getTranslation();
    bias.X += m_rootShift.x();
    bias.Y += m_rootShift.y();
    bias.Z += m_rootShift.z();
    rootAt.setTranslation( bias );

    core::matrix4 rootInv;
    bool res = rootAt.getInverse( rootInv );
    if ( !res )
        return;

    // Parse all bones and add something there.
    core::array<ISkinnedMesh::SJoint*> & joints = mesh->getAllJoints();
    int sz = joints.size();

    for ( int i=0; i<sz; i++ )
    {
        ISkinnedMesh::SJoint * sj = joints[i];

        IBoneSceneNode * jn = node->getJointNode( i );
        core::matrix4 absAt = jn->getAbsoluteTransformation();
        core::matrix4 relAt = rootInv * absAt;

        btTransform tr;
        btTransformFromIrrlichtMatrix( relAt, tr );

        btTransform identity;
        identity.setIdentity();

        btFixedConstraint * c = m_constraints[i];
        c->setFrames( tr, identity );
    }
}

void SkeletonBody::cleanup( SkeletonBody * body, irrBulletWorld * world )
{
    world->getPointer()->removeCollisionObject( body->m_rootBody );

    int qty = static_cast<int>( body->m_constraints.size() );
    for ( int i=0; i<qty; i++ )
    {
        btFixedConstraint * co = body->m_constraints[i];
        world->getPointer()->removeConstraint( co );
        delete co;
    }
    qty = static_cast<int>( body->m_bodies.size() );
    for ( int i=0; i<qty; i++ )
    {
        btRigidBody * b = body->m_bodies[i];
        world->getPointer()->removeRigidBody( b );
        delete b;
    }
    delete body->m_rootBody;
    body->node->drop();
}

