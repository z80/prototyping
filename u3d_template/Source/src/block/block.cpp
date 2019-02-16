
#include "block.h"
#include "name_generator.h"
#include <stack>

using namespace Urho3D;

namespace Osp
{


static const StringHash STRING_HASH( "Block" );

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
        // Right now I don't know how to show/hide
        // nodes :(
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

static Node * getRoot( Node * n );
static void dfsBlocks( std::stack< Node * > & stack, std::vector< Block * > & nodes );

Block * Block::tryAttach()
{
    // First search for a Node not containing Blocks.
    // That is supposed to be a parent. E.i. Assembly scene.
    Node * root = getRoot( GetNode() );
    std::stack< Node * > stack;
    stack.push( root );
    std::vector< Block * > blocks;
    dfsBlocks( stack, blocks );


    const size_t localMakersQty = pivots.size();
    // Check if it has at least one pivot. Ad get radius from there.
    SharedPtr<PivotMarker> p = pivots[0];
    if ( !p )
        return nullptr;
    const float R = p->GetNode()->GetScale().x_ * 2.0f;

    const size_t blocksQty      = blocks.size();
    float      minDist = -1.0f;
    size_t     bestLocalMarkerInd  = 0;
    size_t     bestBlockInd        = 0;
    size_t     bestRemoteMarkerInd = 0;
    for ( size_t localInd=0; localInd<localMakersQty; localInd++ )
    {
        for ( size_t blockInd=0; blockInd<blocksQty; blockInd++ )
        {
            PivotMarker * localMarker = this->pivots[localInd];
            Block * block = blocks[blockInd];
            if ( block == this )
                continue;

            // And also check if this block is a parent of the "block".
            // It it is the case skip it. Because it doesn't make sense
            // to make blocks each other parents.
            const bool isParentOf = this->isParentOf( block );
            if ( isParentOf )
                continue;

            const size_t markersQty = block->pivots.size();
            for ( size_t markerInd=0; markerInd<markersQty; markerInd++ )
            {
                PivotMarker * marker = block->pivots[markerInd];
                Vector3    r;
                Quaternion q;
                const bool relPoseOk = localMarker->relativePose( marker, r, q );
                if ( !relPoseOk )
                    continue;

                const float d = r.Length();
                if ( ( minDist < 0.0 ) || (d < minDist) )
                {
                    bestLocalMarkerInd  = localInd;
                    bestBlockInd        = blockInd;
                    bestRemoteMarkerInd = markerInd;
                    minDist             = d;
                }
            }
        }
    }
    if ( minDist < 0.0f )
        return nullptr;

    if ( minDist > R )
        return nullptr;

    // Compute what relative position should be.
    // Alost compute relative orientation. Reparent
    // and set position and orientation.
    Block       * parentBlock  = blocks[bestBlockInd];
    PivotMarker * localMarker  = this->pivots[bestLocalMarkerInd];
    PivotMarker * parentMarker = parentBlock->pivots[bestRemoteMarkerInd];
    Vector3    r;
    Quaternion q;
    const bool relPoseOk = localMarker->relativePose( parentMarker, r, q );
    // It can't be this way. But just in case.
    if ( !relPoseOk )
        return nullptr;
    this->setParent( parentBlock, true );

    // Compute position.
    const Vector3 parentR = parentMarker->relR();
    Vector3       localR  = localMarker->relR();
    localR = q * localR;
    localR = parentR - localR;
    this->setR( localR );
    this->setQ( q );

    // And set mutual pivot refs.
    parentMarker->connectedTo = SharedPtr<PivotMarker>( localMarker );
    localMarker->connectedTo  = SharedPtr<PivotMarker>( parentMarker );

    return parentBlock;
}

bool    Block::detach()
{
    Node * root = getRoot( GetNode() );
    if ( !root )
        return false;

    const size_t markersQty = pivots.size();
    for ( size_t i=0; i<markersQty; i++ )
    {
        PivotMarker * m = pivots[i];
        PivotMarker * p = m->connectedTo;
        if ( p )
            p->connectedTo = SharedPtr< PivotMarker >( nullptr );
        m->connectedTo = SharedPtr< PivotMarker >( nullptr );
    }

    setParent( root );

    return true;
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



static Node * getRoot( Node * n )
{
    Node * root = n->GetParent();
    while ( true )
    {
        Component * c = root->GetComponent( STRING_HASH );
        if ( !c )
            break;
        root = root->GetParent();
        if ( !root )
            return nullptr;
    }

    return root;
}

static void dfsBlocks( std::stack< Node * > & stack,
                       std::vector< Block * > & nodes )
{
    Node * n = stack.top();

    const Vector< SharedPtr<Node> > & nn = n->GetChildren();
    size_t qty = nn.Size();
    for ( size_t i=0; i<qty; i++ )
    {
        Node * childNode = nn[i];
        Component * c = childNode->GetComponent( STRING_HASH );
        if ( !c )
            continue;
        Block * b = dynamic_cast< Block * >( c );
        nodes.push_back( b );
        stack.push( childNode );
        dfsBlocks( stack, nodes );
    }

    stack.pop();
}


}



