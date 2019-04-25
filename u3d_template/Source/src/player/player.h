
#ifndef __PLAYER_H_
#define __PLAYER_H_

#include <Urho3D/Urho3DAll.h>

#include "assembly.h"
#include "block.h"
#include "planet_base.h"
#include "physics_world_2.h"
#include "game_data.h"

namespace Osp
{

using namespace Urho3D;



class Player: public Urho3D::LogicComponent
{
    URHO3D_OBJECT( Player, LogicComponent )
public:
    Player( Context * ctx );
    ~Player();

    void Start() override;
    /// Called before the first update. At this point all other components of the node should exist. Will also be called if update events are not wanted; in that case the event is immediately unsubscribed afterward.
    void DelayedStart() override;
    /// Called when the component is detached from a scene node, usually on destruction. Note that you will no longer have access to the node and scene at that point.
    void Stop() override;
    /// Called on scene update, variable timestep.
    void Update(float timeStep) override;
    /// Called on physics post-update, fixed timestep.
    void FixedPostUpdate(float timeStep) override;

    /// And mouse reaction.
    void HandleMouseDown( StringHash t, VariantMap & e );
    void HandleMouseUp( StringHash t, VariantMap & e );

private:
    // Get the closest parent planet in node hierarchy.
    // When created need to pick a planet and place physicsWorld and
    // itself to be children of the planet.
    static PlanetBase * parentPlanet( Node * n );


    // Block configuration windows open.
    Vector< SharedPtr<UIElement> > windows;
    // Currently selected assembly.
    SharedPtr<Assembly> assembly;
    // Currently selected block.
    SharedPtr<Block> block;
    // Current planet.
    SharedPtr<PlanetBase> planet;
    // Near surface ?
    bool nearSurface;
    // Dyamics world for computing objects dynamics.
    SharedPtr<PhysicsWorld2> physicsWorld;
    // Game data object to exchange global values.
    SharedPtr<GameData>      gameData;
};

}

#endif


