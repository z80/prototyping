
#include "assembly.h"
#include "name_generator.h"
#include "Urho3D/Physics/RigidBody.h"
#include "Urho3D/Physics/Constraint.h"
#include "Urho3D/Physics/PhysicsWorld.h"

namespace Osp
{

Assembly::Assembly( Context * c )
    : LogicComponent( c )
{
}

Assembly::~Assembly()
{
    destroy();
}

Assembly * Assembly::create( Node * root, const Design & d )
{
    if ( !root )
        return nullptr;

    Node * node = root->CreateChild( NameGenerator::Next( "Assembly" ) );

    Assembly * a = node->CreateComponent<Assembly>();
    a->design = d;
    // Create blocks.
    {
        const size_t blocksQty = d.blocks.size();
        for ( size_t i=0; i<blocksQty; i++ )
        {
            const Design::Block & db = d.blocks[i];
            const String & typeName = db.typeName;
            Node * n = node->CreateChild( NameGenerator::Next( typeName ) );
            Component * c = n->CreateComponent( StringHash( typeName ) );
            Block * b = c->Cast<Block>();
            if ( !b )
                continue;
            b->setQ( db.q );
            b->setR( db.r );
            SharedPtr<Block> sb( b );
            a->blocks.Push( sb );
        }
    }

    a->toWorld();
}

bool Assembly::toWorld()
{
    // Check if dynamics world exists there.
    // And if it does parent all blocks to it.
    Node * node = GetNode();
    PhysicsWorld * w = Assembly::getWorld( node );
    if ( !w )
        return false;

    // Traverse all blocks and joints and add those to world.
    {
        const size_t qty = blocks.Size();
        for ( size_t i=0; i<qty; i++ )
        {
            Block * b = blocks[i];
            if ( !b )
                continue;
            // Add block to world. It should create and initialize
            // rigid body and collision shape.
            b->toWorld();
        }
    }
    {
        // Create joints.
        Design & d = design;
        const size_t connsQty = d.joints.size();
        for ( size_t i=0; i<connsQty; i++ )
        {
            const Design::Joint & dj = d.joints[i];
            const int indA = dj.blockA;
            const int indB = dj.blockB;

            SharedPtr<Block> blockA = this->blocks[indA];
            SharedPtr<Block> blockB = this->blocks[indB];

            // For now place joint point in the middle.
            const Vector3 r = blockB->relR();
            Node * node = blockB->GetNode();
            Constraint * c = node->CreateComponent<Constraint>();
            c->SetConstraintType( CONSTRAINT_HINGE );
            c->SetDisableCollision( true );
            c->SetOtherBody( blockA->rigidBody() );
            c->SetWorldPosition( r );

            const Vector3 axis( 0.0, 0.0, 1.0 );
            const Vector2 lim( 0.0, 0.0 );
            c->SetAxis( axis );
            c->SetOtherAxis( axis );
            c->SetHighLimit( lim );
            c->SetLowLimit( lim );

            this->joints.Push( SharedPtr<Constraint>( c ) );
        }
    }

    return true;
}

void Assembly::fromWorld()
{
    Node * root = GetNode();
    // Traverse all blocks and joints and add those to world.
    {
        const size_t qty = blocks.Size();
        for ( size_t i=0; i<qty; i++ )
        {
            Block * b = blocks[i];
            if ( !b )
                continue;
            // Remove block from world. It should destroy
            // all dynamics related objects (rigid body and collision shape).
            b->fromWorld();
            Node * bn = b->GetNode();
            bn->SetParent( root );
        }
    }
    {
        const size_t qty = joints.Size();
        for ( size_t i=0; i<qty; i++ )
        {
            Constraint * c = joints[i];
            if ( !c )
                continue;
            c->Remove();
        }
        joints.Clear();
    }
}

void Assembly::Update( float timeStep )
{

}

void Assembly::PostUpdate( float timeStep )
{
    // Here recompute position and orientation.
    updatePoseInWorld();
}

void Assembly::destroy()
{
    {
        const size_t qty = blocks.Size();
        for ( size_t i=0; i<qty; i++ )
        {
            Block * b = blocks[i];
            if ( b )
                b->Remove();
        }
    }

    {
        const size_t qty = joints.Size();
        for ( size_t i=0; i<qty; i++ )
        {
            Constraint * c = joints[i];
            if ( c )
                c->Remove();
        }
    }
}

bool Assembly::updatePoseInWorld()
{
    // Retrieve all the blocks.
    Vector3 r( Vector3::ZERO );
    const size_t qty = blocks.Size();
    for ( size_t i=0; i<qty; i++ )
    {
        SharedPtr<Block> sb = blocks[i];
        if ( !sb )
        {
            URHO3D_LOGERROR( "Assembly::updatePoseInWorld() one of blocks doesn\'t exist!" );
            continue;
        }
        const Vector3 ri = sb->relR();
        r += ri;
    }
    const float _1_qty = 1.0/static_cast<float>( qty );
    r = r * _1_qty;
    Node * n = GetNode();
    n->SetPosition( r );

    return true;
}

PhysicsWorld * Assembly::getWorld( Node * node )
{
    Scene * s = node->GetScene();
    if ( !s )
        return nullptr;
    PhysicsWorld * w = s->GetComponent<PhysicsWorld>();
    return w;
}


}

