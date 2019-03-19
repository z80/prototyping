
#include "assembly.h"
#include "name_generator.h"

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

void Assembly::create( const Design & d )
{
    Node * node = GetNode();
    const size_t blocksQty = d.blocks.size();
    for ( size_t i=0; i<blocksQty; i++ )
    {
        Design::Block & db = d.blocks[i];
        const String & typeName = db.typeName;
        Node * n = node->CreateChild( NameGenerator::Next( typeName ) );
        Component * c = n->CreateComponent( StringHash( typeName ) );
        Block * b = c->Cast<Block>();
        if ( !b )
            continue;
        b->setQ( db.q );
        b->setR( db.r );
    }
}

bool Assembly::toWorld()
{
    // Check if dynamics world exists there.
    // And if it does parent all blocks to it.
    return false;
}

void Assembly::fromWorld()
{

}

void Assembly::destroy()
{
    const size_t qty = blocks.size();
    for ( size_t i=0; i<qty; i++ )
    {
        Block * b = blocks[i];
        if ( b )
            b->Remove();
    }
}


}

