
#ifndef __PLAYER_H_
#define __PLAYER_H_

#include "Ogre.h"
#include "entity_part.h"

namespace Osp
{

class Block;

struct Action
{
    Ogre::int32  vehicleId;
    Ogre::int32  actionGroupId;
    Ogre::String name;
    Ogre::Real   value;
};

class Player
{
public:
    Player();
    ~Player();

    virtual bool local() const = 0;

    virtual int  actionsQty() const   = 0;
    virtual bool action( Action & a ) = 0;

    EntityPartPtr activePart;
};

}

#endif


