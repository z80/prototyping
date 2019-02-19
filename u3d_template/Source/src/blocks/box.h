
#ifndef __BOX_H_
#define __BOX_H_

#include "block.h"

namespace Osp
{

class Box: public Block
{
    URHO3D_OBJECT( Box, Block )
public:
    Box( Context * c );
    ~Box();

    SharedPtr<StaticModel> visualModel;
};


}


#endif

