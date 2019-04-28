
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

class Assembly: public LogicComponent
{
    URHO3D_OBJECT( Assembly, Component )
public:
    Assembly( Context * c );
    ~Assembly();

    static Assembly * create( Node * root, const Design & d );
    bool toWorld( PhysicsWorld2 * world );
    void fromWorld( PlanetBase * planet );

    void drawDebugGeometry( DebugRenderer * debug );

protected:
    void Update(float timeStep) override;
    void PostUpdate(float timeStep) override;

public:
    /// Cleanup all blocks and joints.
    void destroy();
    /// If in world nodes are integrated by dynamics world.
    /// According to all node positions update node position
    /// and orientation.
    bool updatePoseInWorld();


    Vector< SharedPtr<Block> >       blocks;
    Vector< SharedPtr<Constraint2> > joints;
    SharedPtr<PlanetBase>            planet;
    Design design;

    bool inAtmosphere;
    bool onSurface;

public:
    static PhysicsWorld2 * getWorld( Node * node );
};

}

#endif




