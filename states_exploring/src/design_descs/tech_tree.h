
#ifndef __TECH_TREE_H_
#define __TECH_TREE_H_

#include "Ogre.h"
#include "part_desc.h"
#include "group_item.h"

namespace Config
{
    class ConfigReader;
}

namespace Entity
{

struct TechNode
{
    /// If it is enabled;
    bool                 enabled;
    /// If it is one step away from enabled node
    /// it has connection with.
    bool                 enableable;
    /// Position.
    Ogre::Vector2        at;
    /// Unique name.
    Ogre::String         name;
    /// Parent names.
    std::vector<Ogre::String> parents;

    /// Icon.
    Ogre::String         iconDisabled;
    Ogre::ResourceHandle iconDisabledHandle;
    Ogre::String         iconHovered;
    Ogre::ResourceHandle iconHoveredHandle;
    Ogre::String         iconEnabled;
    Ogre::ResourceHandle iconEnabledHandle;
};

class TechTree
{
public:
    TechTree();
    ~TechTree();

    bool load( Config::ConfigReader * cr );
    void generateAvalilableParts();
    bool partAllowed( const Ogre::String & name );
    bool enableNode( const Ogre::String & name ) const;

public:
    std::vector<TechNode>     nodes;
    std::vector<PartDesc>     partDescs;
    std::vector<Ogre::String> allowedParts;
};


}


#endif



