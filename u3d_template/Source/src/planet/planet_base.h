
#ifndef __PLANET_BASE_H_
#define __PLANET_BASE_H_

#include <Urho3D/Urho3DAll.h>
#include "item_base.h"
#include "block.h"
#include "assembly.h"
#include "data_types.h"
#include "kepler_mover.h"
#include "kepler_rotator.h"

namespace Osp
{

using namespace Urho3D;

class PlanetBase: public ItemBase
{
    URHO3D_OBJECT( PlanetBase, ItemBase )
public:
    PlanetBase( Context * ctx );
    virtual ~PlanetBase();

    virtual void Start() override;
    virtual void drawDebugGeometry( DebugRenderer * debug );

    /// When player is on the surface the dynamics is
    /// simulated for objects close to the center and close enough
    /// Objects in atmosphere but far away are not simulated.
    virtual void updateCollisionObjects( PhysicsWorld2 * w2, const Vector3d & center, const Vector3 & dist );
    /// Initialize collision objects when player enteres this area.
    virtual void initCollisions( PhysicsWorld2 * w2, const Vector3d & center, Float dist );
    /// Finalize collisions as player leaves the area.
    virtual void finitCollisions( PhysicsWorld2 * w2 );

public:
    bool tryAddSurfaceItem( Node * n );
    bool tryAddOrbitingItem( Node * n );
    void subscribeToParentChanges();
    void OnNodeRemoved( StringHash eventType, VariantMap & eventData );
    void OnNodeAdded( StringHash eventType, VariantMap & eventData );

    // There are at least 3 nodes.
    // 1-st translating, moving the orbit.
    // 2-d  attached to the 1-st one but also rotating. Physcis world and collision bodies
    // should be attached to this one. And atmosphere effects should also be taken into
    // account here.
    // 3-d, 4-th, etc. for graphics.
    // Need only a pointer to the 2-d one here as 1-st one can be obtained by "GetNode()".
    SharedPtr<Node>  dynamicsNode;
    // Yes, can obtain all these through "getComponents" recursive call but will spend
    // some time on casting types in this case.
    Vector<SharedPtr<Block> >    surfaceBlocks,
                                 orbitingBlocks;
    Vector<SharedPtr<Assembly> > orbitingAssemblies;

    // Planet mover and rotator.
    // Mover updates the very root node.
    SharedPtr<KeplerMover>   mover;
    // Rotator updates only "dynamicsNode".
    SharedPtr<KeplerRotator> rotator;
};

}



#endif

