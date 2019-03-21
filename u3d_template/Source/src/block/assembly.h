
#ifndef __ASSEMBLY_H_
#define __ASSEMBLY_H_

#include "block.h"
#include "design.h"

using namespace Urho3D;

namespace Urho3D
{
    class Constraint2;
}

namespace Osp
{

class Assembly: public Component
{
    URHO3D_OBJECT( Assembly, Component )
public:
    Assembly( Context * c );
    ~Assembly();

    static Assembly * create( Node * root, const Design & d );
    bool toWorld();
    void fromWorld();

public:
    void destroy();
    bool inDynamicsWorld;
    Vector< SharedPtr<Block> >       blocks;
    Vector< SharedPtr<Constraint2> > joints;

};

}

#endif




