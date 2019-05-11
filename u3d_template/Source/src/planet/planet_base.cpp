
#include "planet_base.h"
#include "game_data.h"

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

Float PlanetBase::GM() const
{
    return forces->GM_;
}

Vector3d PlanetBase::relR() const
{
    return mover->relR();
}

Quaterniond PlanetBase::relQ() const
{
    return rotator->relQ();
}

Vector3d PlanetBase::relV() const
{
    return mover->relV();
}

Vector3d PlanetBase::relW() const
{
    return rotator->relW();
}

void PlanetBase::Start()
{
    Node * n = GetNode();
    dynamicsNode = n->CreateChild( "DynamicsNode" );
    mover = n->CreateComponent<KeplerMover>();
    //mover = dynamicsNode->CreateComponent<KeplerMover>();
    rotator = dynamicsNode->CreateComponent<KeplerRotator>();

    Scene * s = GetScene();
    Component * c = s->GetComponent( StringHash( "GameData" ), true );
    if ( c )
    {
        GameData * gd = c->Cast<GameData>();
        gd->planets.Push( SharedPtr<PlanetBase>( this ) );
    }

    forces = n->CreateComponent<PlanetForces>();
}

void PlanetBase::drawDebugGeometry( DebugRenderer * debug )
{
    /*{
        const unsigned qty = surfaceBlocks.Size();
        for ( unsigned i=0; i<qty; i++ )
        {
            SharedPtr<Block> b = surfaceBlocks[i];
            b->drawDebugGeometry( debug );
        }
    }*/
}

void PlanetBase::updateCollisionObjects( PhysicsWorld2 * w2, const Vector3d & center, const Vector3 & dist )
{

}

void PlanetBase::initCollisions( PhysicsWorld2 * w2, const Vector3d & center, Float dist )
{

}

void PlanetBase::finitCollisions( PhysicsWorld2 * w2 )
{

}

void PlanetBase::addSurfaceBlock( Block * b )
{
    const unsigned qty = surfaceBlocks.Size();
    for ( unsigned i=0; i<qty; i++ )
    {
        SharedPtr<Block> bb = surfaceBlocks[i];
        if ( bb == b )
            return;
    }
    surfaceBlocks.Push( SharedPtr<Block>(b) );
}

void PlanetBase::removeSurfaceBlock( Block * b )
{
    const unsigned qty = surfaceBlocks.Size();
    for ( unsigned i=0; i<qty; i++ )
    {
        SharedPtr<Block> bb = surfaceBlocks[i];
        if ( bb == b )
        {
            unsigned lastInd = qty-1;
            if ( i != lastInd )
                surfaceBlocks[i] = surfaceBlocks[lastInd];
            surfaceBlocks.Resize( lastInd );
            return;
        }
    }
}

void PlanetBase::addOrbitingAssembly( Assembly * a )
{
    const unsigned qty = orbitingAssemblies.Size();
    for ( unsigned i=0; i<qty; i++ )
    {
        SharedPtr<Assembly> aa = orbitingAssemblies[i];
        if ( aa == a )
            return;
    }
    orbitingAssemblies.Push( SharedPtr<Assembly>(a) );
}

void PlanetBase::removeOrbitingAssembly( Assembly * a )
{
    const unsigned qty = orbitingAssemblies.Size();
    for ( unsigned i=0; i<qty; i++ )
    {
        SharedPtr<Assembly> aa = orbitingAssemblies[i];
        if ( aa == a )
        {
            unsigned lastInd = qty-1;
            if ( i != lastInd )
                orbitingAssemblies[i] = orbitingAssemblies[lastInd];
            orbitingAssemblies.Resize( lastInd );
            return;
        }
    }
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


