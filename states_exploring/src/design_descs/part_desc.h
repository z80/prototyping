
#ifndef __PART_DESC_H_
#define __PART_DESC_H_

#include "Ogre.h"
#include "config_reader.h"
#include "air_mesh.h"
#include "btBulletDynamicsCommon.h"
#include "BtOgrePG.h"
#include "BtOgreGP.h"
#include "BtOgreExtras.h"
#include "OgreOggSoundManager.h"

namespace Desc
{

struct ConnectionDesc
{
    /// Where on a part connection point is.
    Ogre::Vector3    r;
    /// How to orient a part to be connected.
    Ogre::Quaternion q;
    /// If spinning is allowed this normal
    /// for the connection point  is (0, 1, 0) rotated by "q".
    /// And spinning is around this axis.
    bool spinAllowed;
    /// Allowed to be rotated around axes perpendicular to local "y".
    bool tilt;
    /// Allowed to be connected to any surface.
    bool anySurface;
};

struct PartDesc
{
    /// Part name. It should correspond to a
    /// part PartsManager is able to create.
    Ogre::String name;
    Ogre::String category;

    Ogre::String tooltip;
    Ogre::String description;

    Ogre::String         icon;
    Ogre::ResourceHandle iconHandle;

    std::vector<ConnectionDesc> connections;
};




void luaopen_partdesc( lua_State * L );


}

#endif





