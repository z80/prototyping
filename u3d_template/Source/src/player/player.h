
#ifndef __DESIGN_H_
#define __DESIGN_H_

#include <Urho3D/Urho3DAll.h>

namespace Osp
{

using namespace Urho3D;

class Assembly;
class Block;


class Player: public Urho3D::LogicComponent
{
    URHO3D_OBJECT( Player, LogicComponent )
public:
    Player( Context * ctx );
    ~Player();

    /// Called before the first update. At this point all other components of the node should exist. Will also be called if update events are not wanted; in that case the event is immediately unsubscribed afterward.
    void DelayedStart();
    /// Called when the component is detached from a scene node, usually on destruction. Note that you will no longer have access to the node and scene at that point.
    void Stop();
    /// Called on scene update, variable timestep.
    void Update(float timeStep);
    /// Called on physics post-update, fixed timestep.
    void FixedPostUpdate(float timeStep);

    /// And mouse reaction.
private:
    // Block configuration windows open.
    Vector< SharedPtr<UIElement> > windows;
    // Currently selected assembly.
    SharedPtr<Assembly> assembly;
    // Currently selected block.
    SharedPtr<Block> block;
};


