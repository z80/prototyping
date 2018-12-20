
#ifndef __GROUP_ITEM_H_
#define __GROUP_ITEM_H_

#include "Ogre.h"

namespace Group
{

struct GroupDesc
{
    std::string name;
    std::string tooltip;
    std::string description;
};

struct GroupItem
{
    std::string name;
    std::string tooltip;
    std::string description;
    Ogre::ResourceHandle icon;
    int level;
};

struct Group
{
    GroupDesc groupDesc;
    std::vector<GroupItem> items;
};


}

#endif



