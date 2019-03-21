
#include "assembly.h"
#include "name_generator.h"
#include "constraint_2.h"

namespace Osp
{

Assembly::Assembly( Context * c )
    : Component( c )
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

    // Create connections.
    {
        const size_t connsQty = d.joints.size();
        for ( size_t i=0; i<connsQty; i++ )
        {
            const Design::Joint & dj = d.joints[i];
            const int indA = dj.blockA;
            const int indB = dj.blockB;

            SharedPtr<Block> blockA = a->blocks[indA];
            SharedPtr<Block> blockB = a->blocks[indB];

            // For now place joint point in the middle.
            const Vector3 r = blockB->relR();
            Node * node = blockB->GetNode();
            Constraint2 * c = node->CreateComponent<Constraint2>();
            c->SetConstraintType( CONSTRAINT_POINT_2 );
            c->SetDisableCollision( true );
            c->SetOtherBody( blockA->rigidBody() );
            c->SetWorldPosition( r );

            const Vector3 axis( 0.0, 0.0, 1.0 );
            const Vector2 lim( 0.0, 0.0 );
            c->SetAxis( axis );
            c->SetOtherAxis( axis );
            c->SetHighLimit( lim );
            c->SetLowLimit( lim );

            a->joints.Push( SharedPtr<Constraint2>( c ) );
        }
    }
}

bool Assembly::toWorld()
{
    // Check if dynamics world exists there.
    // And if it does parent all blocks to it.

    // Traverse all blocks and joints and add those to world.

    return false;
}

void Assembly::fromWorld()
{

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


}

