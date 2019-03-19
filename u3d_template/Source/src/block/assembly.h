
#ifndef __ASSEMBLY_H_
#define __ASSEMBLY_H_

#include "block.h"

using namespace Urho3D;

namespace Osp
{

class Assembly: public Component
{
    URHO3D_OBJECT( Assembly, Component )
public:
    Assembly( Context * c );
    ~Assembly();


};

}





