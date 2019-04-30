
#include "assembly.h"
#include "name_generator.h"
//#include "Urho3D/Physics/RigidBody.h"
//#include "Urho3D/Physics/Constraint.h"
//#include "Urho3D/Physics/PhysicsWorld.h"
#include "rigid_body_2.h"
#include "constraint_2.h"
#include "physics_world_2.h"
#include "planet_base.h"
#include "game_data.h"


namespace Osp
{

Assembly::Assembly( Context * c )
    : ItemBase( c )
{
    inAtmosphere = true;
    onSurface    = true;
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

    return a;
}

bool Assembly::toWorld( PlanetBase * planet, PhysicsWorld2 * world )
{
    PhysicsWorld2 * w = world;
    if ( !w )
        return false;
    Node * worldNode = w->GetNode();

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
            //b->toWorld();
            Node * blockNode = b->GetNode();

            // Just to check that relative pose is correct.
            //Vector3d rel_r;
            //Quaterniond rel_q;
            //b->relativePose( worldNode, rel_r, rel_q );

            blockNode->SetParent( worldNode );
            planet->addSurfaceBlock( b );
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
            const Vector3d r = blockB->relR();
            Node * node = blockB->GetNode();
            Constraint2 * c = node->CreateComponent<Constraint2>();
            c->SetConstraintType( CONSTRAINT_HINGE_2 );
            c->SetDisableCollision( true );
            c->SetOtherBody( blockA->rigidBody() );
            c->SetWorldPosition( Vector3( r.x_, r.y_, r.z_ ) );

            const Vector3 axis( 0.0, 0.0, 1.0 );
            const Vector2 lim( 0.0, 0.0 );
            c->SetAxis( axis );
            c->SetOtherAxis( axis );
            c->SetHighLimit( lim );
            c->SetLowLimit( lim );

            this->joints.Push( SharedPtr<Constraint2>( c ) );
        }
    }

    Node * assemblyNode = GetNode();
    assemblyNode->SetParent( worldNode );

    return true;
}

void Assembly::fromWorld( PlanetBase * planet )
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
            Constraint2 * c = joints[i];
            if ( !c )
                continue;
            c->Remove();
        }
        joints.Clear();
    }
}

void Assembly::drawDebugGeometry( DebugRenderer * debug )
{
    unsigned qty = blocks.Size();
    for ( unsigned i=0; i<qty; i++ )
    {
        SharedPtr<Block> b = blocks[i];
        b->drawDebugGeometry( debug );
    }
}

void Assembly::Update( float timeStep )
{

}

void Assembly::PostUpdate( float timeStep )
{
    // Here recompute position and orientation.
    updatePoseInWorld();

    // Draw debug geometry.
    Scene * s = GetScene();
    DebugRenderer * debug = s->GetOrCreateComponent<DebugRenderer>();
    if ( debug )
        drawDebugGeometry( debug );
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
            Constraint2 * c = joints[i];
            if ( c )
                c->Remove();
        }
    }
}

bool Assembly::updatePoseInWorld()
{
    // Retrieve all the blocks.
    Vector3d r( Vector3d::ZERO );
    const size_t qty = blocks.Size();
    for ( size_t i=0; i<qty; i++ )
    {
        SharedPtr<Block> sb = blocks[i];
        if ( !sb )
        {
            URHO3D_LOGERROR( "Assembly::updatePoseInWorld() one of blocks doesn\'t exist!" );
            continue;
        }
        const Vector3d ri = sb->relR();
        r += ri;
    }
    const float _1_qty = 1.0/static_cast<float>( qty );
    r = r * _1_qty;
    Node * n = GetNode();
    n->SetPosition( Vector3( r.x_, r.y_, r.z_ ) );

    return true;
}

PhysicsWorld2 * Assembly::getWorld( Node * node )
{
    Node * pn = node->GetParent();
    if ( !pn )
        return nullptr;
    PhysicsWorld2 * w = pn->GetComponent<PhysicsWorld2>();
    return w;
}


}

