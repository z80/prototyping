
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
    /// Category part goes to in the panel.
    Ogre::String category;
    /// Needed tech tree node to allow part
    /// to be visible.
    Ogre::String neededNode;

    Ogre::String tooltip;
    Ogre::String description;

    Ogre::String         icon;
    Ogre::ResourceHandle iconHandle;

    std::vector<ConnectionDesc> connections;
};


// Represents a parts group.
struct CategoryDesc
{
    Ogre::String name;
    Ogre::String tooltip;
    Ogre::String description;

    Ogre::String         icon;
    Ogre::ResourceHandle iconHandle;
};

// Group container.
struct Category
{
    CategoryDesc groupDesc;
    /// PartDesc indices in the array of all PartDescs.
    std::vector<int> items;
};

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
    void generatePanelContent();
    bool enableNode( const Ogre::String & name ) const;

public:
    std::vector<TechNode>     nodes;
    std::vector<PartDesc>     partDescs;
    std::vector<Category>     panelContent;
};


}


#endif



