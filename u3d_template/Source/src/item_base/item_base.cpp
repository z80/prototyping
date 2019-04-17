
#include "item_base.h"
#include "name_generator.h"
#include "SceneManager.h"

using namespace Urho3D;

namespace Osp
{

ItemBase::ItemBase( Context * context )
    : LogicComponent( context )
{
    _type     = TUnspecified;
}

ItemBase::~ItemBase()
{
}

ItemBase::Type ItemBase::type()
{
    return _type;
}

void ItemBase::setR( const Vector3 & at )
{
    Node * n = GetNode();
    n->SetPosition( at );
}

Vector3 ItemBase::relR() const
{
    Node * n = GetNode();
    return n->GetPosition();
}

void ItemBase::setQ( const Quaternion & q )
{
    Node * n = GetNode();
    n->SetRotation( q );
}

Quaternion ItemBase::relQ() const
{
    Node * n = GetNode();
    return n->GetRotation();
}

void ItemBase::setV( const Vector3 & v )
{
}

Vector3 ItemBase::relV() const
{
    return Vector3::ZERO;
}

void ItemBase::setW( const Vector3 & w )
{
}

Vector3 ItemBase::relW() const
{
    return Vector3::ZERO;
}


void ItemBase::setParent( ItemBase * parent, bool inheritRotation )
{
    Vector3    rel_r;
    Quaternion rel_q;
    const bool relPoseOk = relativePose( parent, rel_r, rel_q );

    GetNode()->SetParent( parent->GetNode() );

    if ( relPoseOk )
    {
        setR( rel_r );
        setQ( rel_q );
    }
}

void ItemBase::setParent( Node * parent, bool inheritRotation )
{
    Vector3    rel_r;
    Quaternion rel_q;
    const bool relPoseOk = relativePose( parent, rel_r, rel_q );

    GetNode()->SetParent( parent );

    if ( relPoseOk )
    {
        setR( rel_r );
        setQ( rel_q );
    }
}

bool ItemBase::isParentOf( ItemBase * item ) const
{
    Node * n = GetNode();
    Scene * scene = GetScene();
    const Node * root = scene;
    Node * node = item->GetNode();
    while ( ( node != root ) && ( node ) )
    {
        if ( n == node )
            return true;
        node = node->GetParent();
    }

    return false;
}

ItemBase * ItemBase::parentItem() const
{
    Node * n = GetNode()->GetParent();
    if ( !n )
        return 0;

    static Vector< SharedPtr<Component> > comps;
    comps = n->GetComponents();
    const size_t qty = comps.Size();
    for ( size_t i=0; i<qty; i++ )
    {
        ItemBase * ib = comps[i]->Cast<ItemBase>();
        if ( !ib )
            continue;
        return ib;
    }

    return nullptr;
}

bool ItemBase::relativePose( ItemBase * other, Vector3 & rel_r, Quaternion & rel_q )
{
    // root->a->b->c->d->e->this
    // root->a->b->other->f->g
    // The idea is to accumulate relative position and orientation.
    Vector3    r = Vector3::ZERO;
    Quaternion q = Quaternion::IDENTITY;

    const Node * root = GetScene();

    // Get all ancestors of current node.
    // Make it static as graphics is in one thread.
    static std::vector<Node *> allAncestorsA;
    allAncestorsA.clear();
    Node * nodeA = this->GetNode();
    do {
        allAncestorsA.push_back( nodeA );
        nodeA = nodeA->GetParent();
    } while ( nodeA );
    const size_t allQtyA = allAncestorsA.size();

    Node * nodeB = other->GetNode();
    static std::vector<Node *> ancestorsB;
    ancestorsB.clear();
    size_t indA = allQtyA;
    do {
        ancestorsB.push_back( nodeB );
        // Check if nodeB is in allAncestorsA.
        for ( size_t i=0; i<allQtyA; i++ )
        {
            nodeA = allAncestorsA[i];
            if ( nodeA == nodeB )
            {
                indA = i;
                break;
            }
        }
        if ( indA != allQtyA )
            break;
        // Get parent.
        nodeB = nodeB->GetParent();
    } while ( nodeB );

    // If reached the root and didn't meed
    // anything common just break.
    if ( indA == allQtyA )
        return false;

    // Here there is a closest common ancestor.
    // First find pose of nodeA in it's ref. frame.
    Vector3    ra = Vector3::ZERO;
    Quaternion qa = Quaternion::IDENTITY;
    for ( size_t i=0; i<indA; i++ )
    {
        nodeA = allAncestorsA[i];
        const Quaternion q = nodeA->GetRotation();
        const Vector3    r = nodeA->GetPosition();
        ra = q*ra;
        ra = r + ra;
        qa = q * qa;
    }

    Vector3    rb = Vector3::ZERO;
    Quaternion qb = Quaternion::IDENTITY;
    const size_t indB = ancestorsB.size()-1;
    for ( size_t i=0; i<indB; i++ )
    {
        Node * nodeB = ancestorsB[i];
        const Quaternion q = nodeB->GetRotation();
        const Vector3    r = nodeB->GetPosition();
        rb = q*rb;
        rb = r + rb;
        qb = q * qb;
    }

    rel_r = ra - rb;
    // This might be wrong.
    // I probably don't need quaternion at all.
    rel_q = qb.Inverse() * qa;
    return true;
}

bool ItemBase::relativePose( Node * other, Vector3 & rel_r, Quaternion & rel_q )
{
    // root->a->b->c->d->e->this
    // root->a->b->other->f->g
    // The idea is to accumulate relative position and orientation.
    Vector3    r = Vector3::ZERO;
    Quaternion q = Quaternion::IDENTITY;

    const Node * root = GetScene();

    // Get all ancestors of current node.
    // Make it static as graphics is in one thread.
    static std::vector<Node *> allAncestorsA;
    allAncestorsA.clear();
    Node * nodeA = this->GetNode();
    do {
        allAncestorsA.push_back( nodeA );
        nodeA = nodeA->GetParent();
    } while ( nodeA );
    const size_t allQtyA = allAncestorsA.size();

    Node * nodeB = other;
    static std::vector<Node *> ancestorsB;
    ancestorsB.clear();
    size_t indA = allQtyA;
    do {
        ancestorsB.push_back( nodeB );
        // Check if nodeB is in allAncestorsA.
        for ( size_t i=0; i<allQtyA; i++ )
        {
            nodeA = allAncestorsA[i];
            if ( nodeA == nodeB )
            {
                indA = i;
                break;
            }
        }
        if ( indA != allQtyA )
            break;
        // Get parent.
        nodeB = nodeB->GetParent();
    } while ( nodeB );

    // If reached the root and didn't meed
    // anything common just break.
    if ( indA == allQtyA )
        return false;

    // Here there is a closest common ancestor.
    // First find pose of nodeA in it's ref. frame.
    Vector3    ra = Vector3::ZERO;
    Quaternion qa = Quaternion::IDENTITY;
    for ( size_t i=0; i<indA; i++ )
    {
        nodeA = allAncestorsA[i];
        const Quaternion q = nodeA->GetRotation();
        const Vector3    r = nodeA->GetPosition();
        ra = q*ra;
        ra = r + ra;
        qa = q * qa;
    }

    Vector3    rb = Vector3::ZERO;
    Quaternion qb = Quaternion::IDENTITY;
    const size_t indB = ancestorsB.size()-1;
    for ( size_t i=0; i<indB; i++ )
    {
        Node * nodeB = ancestorsB[i];
        const Quaternion q = nodeB->GetRotation();
        const Vector3    r = nodeB->GetPosition();
        rb = q*rb;
        rb = r + rb;
        qb = q * qb;
    }

    rel_r = ra - rb;
    // This might be wrong.
    // I probably don't need quaternion at all.
    rel_q = qb.Inverse() * qa;
    return true;
}

bool ItemBase::relativePose( Node * n, Node * p, Vector3 & rel_r, Quaternion & rel_q )
{
    // root->a->b->c->d->e->this
    // root->a->b->other->f->g
    // The idea is to accumulate relative position and orientation.
    Vector3    r = Vector3::ZERO;
    Quaternion q = Quaternion::IDENTITY;

    const Node * root = n->GetScene();

    // Get all ancestors of current node.
    // Make it static as graphics is in one thread.
    static std::vector<Node *> allAncestorsA;
    allAncestorsA.clear();
    Node * nodeA = n;
    do {
        allAncestorsA.push_back( nodeA );
        nodeA = nodeA->GetParent();
    } while ( nodeA );
    const size_t allQtyA = allAncestorsA.size();

    Node * nodeB = p;
    static std::vector<Node *> ancestorsB;
    ancestorsB.clear();
    size_t indA = allQtyA;
    do {
        ancestorsB.push_back( nodeB );
        // Check if nodeB is in allAncestorsA.
        for ( size_t i=0; i<allQtyA; i++ )
        {
            nodeA = allAncestorsA[i];
            if ( nodeA == nodeB )
            {
                indA = i;
                break;
            }
        }
        if ( indA != allQtyA )
            break;
        // Get parent.
        nodeB = nodeB->GetParent();
    } while ( nodeB );

    // If reached the root and didn't meed
    // anything common just break.
    if ( indA == allQtyA )
        return false;

    // Here there is a closest common ancestor.
    // First find pose of nodeA in it's ref. frame.
    Vector3    ra = Vector3::ZERO;
    Quaternion qa = Quaternion::IDENTITY;
    for ( size_t i=0; i<indA; i++ )
    {
        nodeA = allAncestorsA[i];
        const Quaternion q = nodeA->GetRotation();
        const Vector3    r = nodeA->GetPosition();
        ra = q*ra;
        ra = r + ra;
        qa = q * qa;
    }

    Vector3    rb = Vector3::ZERO;
    Quaternion qb = Quaternion::IDENTITY;
    const size_t indB = ancestorsB.size()-1;
    for ( size_t i=0; i<indB; i++ )
    {
        Node * nodeB = ancestorsB[i];
        const Quaternion q = nodeB->GetRotation();
        const Vector3    r = nodeB->GetPosition();
        rb = q*rb;
        rb = r + rb;
        qb = q * qb;
    }

    rel_r = ra - rb;
    // This might be wrong.
    // I probably don't need quaternion at all.
    rel_q = qb.Inverse() * qa;
    return true;
}




}





