
#include "block.h"
#include "name_generator.h"

using namespace Urho3D;

namespace Osp
{


Block::Block( Context * c )
    : ItemBase( c )
{

}

Block::~Block()
{

}

void Block::setPivotsVisible( bool en )
{
    const size_t qty = pivots.size();
    for ( size_t i=0; i<qty; i++ )
    {
        PivotMarker * m = pivots[i];
        if ( !m )
            continue;
        //m->model->SetSetEnabled( en );
    }
}

void Block::setPivotSize( float sz )
{
    const size_t qty = pivots.size();
    for ( size_t i=0; i<qty; i++ )
    {
        PivotMarker * m = pivots[i];
        if ( !m )
            continue;
        m->setSize( sz );
    }
}

Block * Block::closestBlock( Node * root, size_t & ownPivot, size_t & otherPivot )
{
    return 0;
}

void Block::createPivots( size_t qty )
{
    Node * node = GetNode();
    pivots.clear();
    pivots.reserve( qty );
    for ( size_t i=0; i<qty; i++ )
    {
        Node * n = node->CreateChild( NameGenerator::Next( "Pivot" ) );
        PivotMarker * c = dynamic_cast<PivotMarker *>( n->CreateComponent( StringHash( "PivotMarker" ) ) );
        SharedPtr<PivotMarker> pm( c );
        pivots.push_back( pm );
    }
}


}



