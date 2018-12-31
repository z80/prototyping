
#ifndef __GROUP_ITEM_H_
#define __GROUP_ITEM_H_

#include "Ogre.h"

namespace Group
{

// Represents a parts group.
struct GroupDesc
{
    Ogre::String name;
    Ogre::String tooltip;
    Ogre::String description;

    Ogre::String         icon;
    Ogre::ResourceHandle iconHandle;
};

// Represents a particular part type.
struct GroupItem
{
    Ogre::String name;
    Ogre::String tooltip;
    Ogre::String description;
    Ogre::String         icon;
    Ogre::ResourceHandle iconHandle;
    int level;
};

// Group container.
struct Group
{
    GroupDesc groupDesc;
    std::vector<GroupItem> items;
};


}

#endif



