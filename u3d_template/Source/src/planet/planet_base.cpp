
#include "planet_base.h"

namespace Osp
{

PlanetBase::PlanetBase( Context * ctx )
    : ItemBase( ctx )
{
    _type = TPlanet;
}

PlanetBase::~PlanetBase()
{

}

void PlanetBase::Start()
{
    Node * n = GetNode();
    dynamicsNode = n->CreateChild( "DynamicsNode" );
    mover = n->CreateComponent<KeplerMover>();
    //mover = dynamicsNode->CreateComponent<KeplerMover>();
    rotator = dynamicsNode->CreateComponent<KeplerRotator>();
}

void PlanetBase::drawDebugGeometry( DebugRenderer * debug )
{
    {
        const unsigned qty = surfaceBlocks.Size();
        for ( unsigned i=0; i<qty; i++ )
        {
            SharedPtr<Block> b = surfaceBlocks[i];
            b->drawDebugGeometry( debug );
        }
    }
}

void PlanetBase::updateCollisionObjects( PhysicsWorld2 * w2, const Vector3d & center, const Vector3 & dist )
{

}

void PlanetBase::initCollisions( PhysicsWorld2 * w2, const Vector3d & center, const Vector3 & dist )
{

}

void PlanetBase::finitCollisions( PhysicsWorld2 * w2 )
{

}


bool PlanetBase::tryAddSurfaceItem( Node * n )
{

    return false;
}

bool PlanetBase::tryAddOrbitingItem( Node * n )
{

    return false;
}

void PlanetBase::subscribeToParentChanges()
{
    SubscribeToEvent( E_NODEREMOVED, URHO3D_HANDLER( PlanetBase, OnNodeRemoved ) );
    SubscribeToEvent( E_NODEADDED,   URHO3D_HANDLER( PlanetBase, OnNodeAdded ) );
}

void PlanetBase::OnNodeRemoved( StringHash eventType, VariantMap & eventData )
{
    Variant & v = eventData[NodeRemoved::P_PARENT];
    Node * msgParent = static_cast<Node *>( v.GetPtr() );
    Node * parent = GetNode();
    if ( msgParent == parent )
    {
        // Remove from orbiting blocks/assemblies.
    }
    else if ( msgParent == dynamicsNode )
    {
        // Remove from surface blocks.
    }

    //Variant & n = eventData[NodeRemoved::P_NODE];
    //Node * self = static_cast<Node *>( n.GetPtr() );
    //Node * node = GetNode();

}

void PlanetBase::OnNodeAdded( StringHash eventType, VariantMap & eventData )
{
    Variant & v = eventData[NodeAdded::P_PARENT];
    Node * msgParent = static_cast<Node *>( v.GetPtr() );
    Node * parent = GetNode();
    if ( msgParent == parent )
    {
        // Adding to orbiting blocks/assemblies.
    }
    else if ( msgParent == dynamicsNode )
    {
        // Adding to surface blocks.
    }
}

}


