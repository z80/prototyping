
#ifndef __WORLD_MOVER_H_
#define __WORLD_MOVER_H_

#include "item_base.h"


namespace Osp
{

class WorldMover: public ItemBase
{
    URHO3D_OBJECT( WorldMover, ItemBase )
public:
    WorldMover( Context * ctx );
    ~WorldMover();




};

}





#endif


