
#include "block.h"
#include "name_generator.h"
#include "tech_tree.h"
#include "assembly.h"

#include "Urho3D/Physics/RigidBody.h"
#include "Urho3D/Physics/CollisionShape.h"

#include <stack>


using namespace Urho3D;

namespace Osp
{


Block::Block( Context * c, const String & name )
    : ItemBase( c ),
      name( name )
{

}

Block::~Block()
{

}

void Block::createContent( Node * n )
{
    // Create pivots.
    placePivots();
}

RigidBody * Block::rigidBody()
{
    Node * n = GetNode();
    RigidBody * rb = n->GetComponent<RigidBody>();
    return rb;
}

CollisionShape * Block::collisionShape()
{
    Node * n = GetNode();
    CollisionShape * cs = n->GetComponent<CollisionShape>();
    return cs;
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
        m->model->SetEnabled( en );
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
    Block * b = tryAttachToConnectionPoint();
    if ( b )
        return b;

    b = tryAttachToSurface();
    return b;
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

Block * Block::parentBlock()
{
    Node * n = GetNode()->GetParent();
    if ( !n )
        return 0;

    static Vector< SharedPtr<Component> > comps;
    comps = n->GetComponents();
    const size_t qty = comps.Size();
    for ( size_t i=0; i<qty; i++ )
    {
        Block * b = comps[i]->Cast<Block>();
        if ( !b )
            continue;
        return b;
    }

    return nullptr;
}

const Vector3 Block::axisToParent()
{
    Block * p = parentBlock();
    if ( !p )
        return Vector3::ZERO;

    const size_t qty = pivots.size();
    for ( size_t i=0; i<qty; i++ )
    {
        PivotMarker * m = pivots[i];
        Block * markerBlock = m->blockConnectedTo();
        if ( markerBlock == p )
        {
            // Need appropriate axis.
            return m->connectionDesc.a;
        }
    }

    return Vector3::ZERO;
}

void Block::placePivots()
{
    TechTree * tt = GetSubsystem( StringHash( "TechTree" ) )->Cast<TechTree>();
    const PartDesc & pd = tt->partDesc( name );
    const size_t qty = pd.connections.size();

    createPivots( qty );
    for ( size_t i=0; i<qty; i++ )
    {
        const ConnectionDesc & cd = pd.connections[i];
        SharedPtr<PivotMarker> pm = pivots[i];
        pm->setR( cd.r );
        pm->connectionDesc = cd;
    }
    // By default hide pivots.
    setPivotsVisible( false );
}

void Block::clearPivots()
{
    const size_t qty = pivots.size();
    for ( size_t i=0; i<qty; i++ )
    {
        SharedPtr<PivotMarker> pm = pivots[i];
        Node * n = pm->GetNode();
        n->Remove();
    }
    pivots.clear();
}

void Block::createPivots( size_t qty )
{
    Node * node = GetNode();
    pivots.clear();
    pivots.reserve( qty );
    for ( size_t i=0; i<qty; i++ )
    {
        Node * n = node->CreateChild( NameGenerator::Next( "Pivot" ) );
        PivotMarker * c = n->CreateComponent( StringHash( "PivotMarker" ) )->Cast<PivotMarker>();
        SharedPtr<PivotMarker> pm( c );
        pivots.push_back( pm );
    }
}

void Block::alignOrientation( const Vector3 & ownA, const Vector3 & parentA )
{
    const Quaternion q = relQ();
    Vector3 a = q*ownA;
    a = -a;
    Quaternion adjQ;
    adjQ.FromRotationTo( a, parentA );
    adjQ = adjQ * q;
    adjQ.Normalize();
    setQ( adjQ );
}


Block * Block::tryAttachToConnectionPoint()
{
    // First search for a Node not containing Blocks.
    // That is supposed to be a parent. E.i. Assembly scene.
    Node * blockNode = GetNode();
    Node * root = getRoot( blockNode );
    std::stack< Node * > stack;
    stack.push( root );
    std::vector< Block * > blocks;
    dfsBlocks( stack, blocks );


    const size_t localMakersQty = pivots.size();
    // Check if it has at least one pivot. Ad get radius from there.
    SharedPtr<PivotMarker> p = pivots[0];
    if ( !p )
        return nullptr;
    const float R = p->GetNode()->GetScale().x_;

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
    alignOrientation( localMarker->connectionDesc.a, parentMarker->connectionDesc.a );
    // After aligning orientation update quaternion.
    q = relQ();
    // Compute position.
    const Vector3 parentR = parentMarker->relR();
    Vector3       localR  = localMarker->relR();
    localR = q * localR;
    localR = parentR - localR;
    this->setR( localR );
    //this->setQ( q );

    // And set mutual pivot refs.
    parentMarker->connectedTo = SharedPtr<PivotMarker>( localMarker );
    localMarker->connectedTo  = SharedPtr<PivotMarker>( parentMarker );


    return parentBlock;
}

Block * Block::tryAttachToSurface()
{
    const size_t localMakersQty = pivots.size();
    if ( localMakersQty == 0 )
        return nullptr;
    // Check if it has at least one pivot. Ad get radius from there.
    SharedPtr<PivotMarker> p = pivots[0];
    const float R = p->modelNode->GetScale().x_;

    Scene * s = GetScene();

    Block * block = 0;
    Vector3 position, normal;
    Vector3 axis;
    PivotMarker * localMarker = 0;
    for ( size_t pivotInd=0; pivotInd<localMakersQty; pivotInd++ )
    {
        localMarker = pivots[pivotInd];
        if ( !localMarker->connectionDesc.attachAnywhere )
            continue;

        axis = localMarker->connectionDesc.a;
        Vector3    rel_r;
        Quaternion rel_q;
        const bool res = localMarker->relativePose( s, rel_r, rel_q );
        const Vector3 origin    = rel_r;
        const Vector3 direction = rel_q * axis;
        const Ray ray( origin, direction );

        PODVector<RayQueryResult> results;
        const float maxDistance = R;
        RayOctreeQuery query( results, ray, RAY_TRIANGLE, maxDistance, DRAWABLE_GEOMETRY );
        s->GetComponent<Octree>()->RaycastSingle( query );
        const size_t qty = results.Size();
        for ( size_t drawableInd=0; drawableInd<qty; drawableInd++ )
        {
            const RayQueryResult & r = results[drawableInd];
            Node * n = r.node_;

            const Vector<SharedPtr<Component> > & comps = n->GetComponents();
            const size_t compsQty = comps.Size();
            for ( size_t compInd=0; compInd<compsQty; compInd++ )
            {
                Component * c = comps[compInd];
                Block * b = c->Cast<Block>();
                if ( b )
                {
                    block = b;
                    position = r.position_;
                    normal   = r.normal_;
                    break;
                }
            }
            if ( block )
               break;
        }

        if ( block )
            break;
    }

    if ( !block )
        return nullptr;


    Vector3    rel_r;
    Quaternion rel_q;
    const bool res = block->relativePose( s, rel_r, rel_q );
    position = rel_q.Inverse() * (position - rel_r);
    normal   = rel_q.Inverse() * normal;

    setParent( block );
    alignOrientation( axis, normal );

    Vector3       localR  = localMarker->relR();
    rel_q = relQ();
    localR = rel_q * localR;
    localR = position - localR;
    this->setR( localR );


    return nullptr;
}

void Block::toWorld()
{
    // Subscribing to world updates to track body state.
    Node * n = GetNode();
    PhysicsWorld * w = Assembly::getWorld( n );
    if ( !w )
        return;
    SubscribeToEvent(w, E_PHYSICSPOSTSTEP, URHO3D_HANDLER( Block, OnPhysicsPostStep ) );
}

void Block::fromWorld()
{
    RigidBody * rb = rigidBody();
    if ( rb )
        rb->Remove();

    CollisionShape * cs = collisionShape();
    if ( cs )
        cs->Remove();

    Node * n = GetNode();
    PhysicsWorld * w = Assembly::getWorld( n );
    if ( !w )
        return;
    UnsubscribeFromEvent( w, E_PHYSICSPOSTSTEP );
}



void Block::OnNodeSet( Node * node )
{
    Component::OnNodeSet( node );
    if ( node )
        createContent( node );
}

void Block::OnPhysicsPostStep( StringHash t, VariantMap & e )
{
    RigidBody * rb = rigidBody();
    if ( !rb )
        return;
    // Need to update dynamical properties (probably...).
    //rb->GetPosition();
    //rb->GetRotation();
    //rb->GetVelocityAtPoint();
    //rb->GetAngularVelocity();
}



static Node * getRoot( Node * n )
{
    Node * root = n->GetParent();
    while ( true )
    {
        const Vector<SharedPtr<Component> > comps = root->GetComponents();
        // Search for a block and if there is no block
        // it is the one.
        const size_t qty = comps.Size();
        Block * b = nullptr;
        for ( size_t i=0; i<qty; i++ )
        {
            Component * c = comps[i];
            b = c->Cast<Block>();
            if ( b )
                break;
        }
        if ( !b )
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

        const Vector<SharedPtr<Component> > & comps = childNode->GetComponents();
        const size_t compsQty = comps.Size();
        Block * b = nullptr;
        for ( size_t i=0; i<compsQty; i++ )
        {
            b = comps[i]->Cast<Block>();
            if ( b )
            {
                nodes.push_back( b );;
                break;
            }
        }

        stack.push( childNode );
        dfsBlocks( stack, nodes );
    }

    stack.pop();
}


}



