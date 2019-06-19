
#include "planet_base.h"
#include "world_mover.h"
#include "physics_world_2.h"
#include "game_data.h"
#include "MyEvents.h"

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

bool PlanetBase::canOrbit( const ItemBase * a ) const
{
    const bool res = forces->canOrbit( a );
    return res;
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

    subscribeToWorldEvents();
}

void PlanetBase::DrawDebugGeometry( DebugRenderer * debug, bool depthTest )
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

void PlanetBase::updateCollisions( PhysicsWorld2 * w2, Osp::WorldMover * mover, Float dist )
{

}

void PlanetBase::initCollisions( PhysicsWorld2 * w2, Osp::WorldMover * mover, Float dist )
{

}

void PlanetBase::finitCollisions( PhysicsWorld2 * w2 )
{

}





void PlanetBase::subscribeToWorldEvents()
{
    SubscribeToEvent( MyEvents::E_WORLD_SWITCHED_ASSEMBLY, URHO3D_HANDLER( PlanetBase, OnWorldSwitchedAssembly ) );
    SubscribeToEvent( MyEvents::E_WORLD_STATE_CHANGED,     URHO3D_HANDLER( PlanetBase, OnWorldStateChanged ) );
    SubscribeToEvent( MyEvents::E_WORLD_MOVED,             URHO3D_HANDLER( PlanetBase, OnWorldMoved ) );
}

void PlanetBase::OnWorldSwitchedAssembly( StringHash eventType, VariantMap & eventData )
{
    using namespace MyEvents::WorldSwitchedAssembly;
    const Variant & po = eventData[ P_PLANET_OLD ];
    const PlanetBase * p_old = (PlanetBase *)po.GetVoidPtr();
    // Also need old world position and velocity to properly initialize movement.
    const Variant & pn = eventData[ P_PLANET_NEW ];
    const PlanetBase * p_new = (PlanetBase *)pn.GetVoidPtr();

    // Get dynamics world.
    if ( !physicsWorld )
    {
        Scene * s = GetScene();
        Component * c = s->GetComponent( StringHash("PhysicsWorld2"), true );
        if ( !c )
            return;
        physicsWorld = SharedPtr<Component>( c );
    }
    if ( !worldMover )
    {
        Scene * s = GetScene();
        Component * c = s->GetComponent( StringHash("WorldMover"), true );
        if ( !c )
            return;
        worldMover = SharedPtr<Component>( c );
    }

    PhysicsWorld2 * pw2 = physicsWorld->Cast<PhysicsWorld2>();
    WorldMover    * wm  = worldMover->Cast<WorldMover>();
    if ( (p_new == this) && (p_old != this) )
    {
        initCollisions( pw2, wm, GameData::DIST_PLANET_COLLISIONS );
    }
    else if ( ( p_old == this ) && ( p_new != this ) )
    {
        finitCollisions( pw2 );
    }
}

void PlanetBase::OnWorldStateChanged( StringHash eventType, VariantMap & eventData )
{
    using namespace MyEvents::WorldStateChanged;
    const Variant & po = eventData[ P_PLANET_OLD ];
    const PlanetBase * p_old = (PlanetBase *)po.GetVoidPtr();
    // Also need old world position and velocity to properly initialize movement.
    const Variant & pn = eventData[ P_PLANET_NEW ];
    const PlanetBase * p_new = (PlanetBase *)pn.GetVoidPtr();

    // Get dynamics world.
    if ( !physicsWorld )
    {
        Scene * s = GetScene();
        Component * c = s->GetComponent( StringHash("PhysicsWorld2"), true );
        if ( !c )
            return;
        physicsWorld = SharedPtr<Component>( c );
    }
    if ( !worldMover )
    {
        Scene * s = GetScene();
        Component * c = s->GetComponent( StringHash("WorldMover"), true );
        if ( !c )
            return;
        worldMover = SharedPtr<Component>( c );
    }

    PhysicsWorld2 * pw2 = physicsWorld->Cast<PhysicsWorld2>();
    WorldMover    * wm  = worldMover->Cast<WorldMover>();
    if ( (p_new == this) && (p_old != this) )
    {
        initCollisions( pw2, wm, GameData::DIST_PLANET_COLLISIONS );
    }
    else if ( ( p_old == this ) && ( p_new != this ) )
    {
        finitCollisions( pw2 );
    }
}

void PlanetBase::OnWorldMoved( StringHash eventType, VariantMap & eventData )
{
    if ( !worldMover )
    {
        Scene * s = GetScene();
        Component * c = s->GetComponent( StringHash("WorldMover"), true );
        if ( !c )
            return;
        worldMover = SharedPtr<Component>( c );
    }

    if ( !physicsWorld )
      return;

    PhysicsWorld2 * pw2 = physicsWorld->Cast<PhysicsWorld2>();
    
    WorldMover * wm = worldMover->Cast<WorldMover>();
    if ( wm->planet != this )
      return;

    updateCollisions( pw2, wm, GameData::DIST_PLANET_COLLISIONS );
}




}


