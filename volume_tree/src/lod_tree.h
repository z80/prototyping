
#ifndef __LOD_TREE_H_
#define __LOD_TREE_H_

#include "Ogre.h"
#include "OgreApplicationContext.h"
#include "OgreInput.h"
#include "OgreRTShaderSystem.h"
#include "OgreCameraMan.h"
#include "OgreTrays.h"
#include "OgreAdvancedRenderControls.h"

#include "volume.h"

#include <iostream>

using namespace Ogre;

namespace LodTree
{

class TreeParams
{
public:
    // Here "baseError" should be for the most rough level.
    ChunkParameters chunkParams;
    // Max tree level.
    int  maxLevel;
};

class Tree;

class Node
{
private:
    Node();
public:
    ~Node();
    Node( const Node & inst );
    const Node & operator=( const Node & inst );

    bool needSubdriven( SceneNode * camNode ) const;
    bool subdrive();

    /// Center.
    Vector3 at;
    /// Half size.
    Real    halfSz;
    // Level of this node. Root node has level 1. Each
    // sublevel has level=level+1.
    int level;
    /// If this one is leaf node.
    bool    leafNode;
    // Subnode pointers.
    Node * nodes[8];
    // Tree pointer.
    Tree * tree;
    // Volume rendering stuff.
    Chunk        * volume;
    SceneNode    * sceneNode;
};

class Tree
{
public:
    Tree( SceneManager * smgr );
    ~Tree();

private:
    SceneManager * smgr;
};

}

#endif



