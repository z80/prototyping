
#ifndef __ASSEMBLY_H_
#define __ASSEMBLY_H_

#include "block.h"
#include "design.h"

using namespace Urho3D;

namespace Urho3D
{
    class Constraint;
    class PhysicsWorld;
}

namespace Osp
{

class Assembly: public LogicComponent
{
    URHO3D_OBJECT( Assembly, Component )
public:
    Assembly( Context * c );
    ~Assembly();

    static Assembly * create( Node * root, const Design & d );
    bool toWorld();
    void fromWorld();

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


    Vector< SharedPtr<Block> >      blocks;
    Vector< SharedPtr<Constraint> > joints;
    Design design;

public:
    static PhysicsWorld * getWorld( Node * node );
};

}

#endif




