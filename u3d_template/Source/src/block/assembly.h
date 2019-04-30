
#ifndef __ASSEMBLY_H_
#define __ASSEMBLY_H_

#include "block.h"
#include "design.h"

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
    bool updatePoseInWorld();
    bool updatePoseInOrbit();

    /// Check influence.
    void checkInfluence();
    PlanetBase * planetOfInfluence();


    Vector< SharedPtr<Block> >       blocks;
    Vector< SharedPtr<Constraint2> > joints;
    SharedPtr< KeplerMover >         mover;

    SharedPtr<PlanetBase>            planet;


    SharedPtr<GameData>              gameData;
    Design design;

    bool inAtmosphere;
    bool onSurface;
    bool inWorld;

public:
    static PhysicsWorld2 * getWorld( Node * node );
};

}

#endif




