
#ifndef __ASSEMBLY_H_
#define __ASSEMBLY_H_

#include "block.h"
#include "design.h"
//#include "world_mover.h"

using namespace Urho3D;

namespace Urho3D
{
    class Constraint2;
    class PhysicsWorld2;
}

namespace Osp
{

class PlanetBase;
class GameData;
class KeplerMover;
//class WorldMover;

class Assembly: public ItemBase
{
    URHO3D_OBJECT( Assembly, ItemBase )
public:
    Assembly( Context * c );
    ~Assembly();

    static Assembly * create( Node * root, const Design & d );
    bool toWorld( PlanetBase * planet, PhysicsWorld2 * world );
    void fromWorld( PlanetBase * planet );

    void drawDebugGeometry( DebugRenderer * debug );

protected:
    void Start() override;
    void Update(float timeStep) override;
    void PostUpdate(float timeStep) override;

public:
    /// Cleanup all blocks and joints.
    void destroy();
    /// If in world nodes are integrated by dynamics world.
    /// According to all node positions update node position
    /// and orientation.
    void updatePoseInWorld();
    void updatePoseInOrbit();

    /// Check influence.
    void checkInfluence();
    PlanetBase * planetOfInfluence();

    /// Check if necessary to enter dynamics world or
    /// leave it.
    bool needLeaveWorld();
    bool needEnterWorld();


    Vector< SharedPtr<Block> >       blocks;
    Vector< SharedPtr<Constraint2> > joints;

    SharedPtr< KeplerMover >         mover;

    SharedPtr<PlanetBase>            planet;
    SharedPtr<GameData>              gameData;

    SharedPtr<ItemBase>              worldMover;

    Design design;

    // In dynamics world.
    bool inWorld;
    // Need to apply friction and gravity
    bool inAtmosphere;
    // If let world in atmosphere only can be on the surface.
    bool onSurface;

    void subscribeToEvents();
    void OnWorldSwitched( StringHash eventType, VariantMap & eventData );
    void OnWorldMoved( StringHash eventType, VariantMap & eventData );
    void adjustMovementInWorld( const Vector3 & dr, const Vector3 & dv );

    static const Float DIST_LEAVE_WORLD;
    static const Float DIST_ENTER_WORLD;

public:
    static PhysicsWorld2 * getWorld( Node * node );
};

}

#endif




