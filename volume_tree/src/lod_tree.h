
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

struct TreeParams: public ChunkParameters
{
public:
    // Here "baseError" should be for the most rough level.
    // Max tree level. And error fofr max level is
    // "baseError" / 2^maxLevel
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
    bool subdrive( SceneNode * camNode );
    bool hasGeometry() const;

    /// Center.
    Vector3 at;
    /// Half size.
    Real    halfSz;
    /// Level of this node. Root node has level 1. Each
    /// sublevel has level=level+1.
    int level;
    /// If this one is leaf node.
    bool    leafNode;
    // Subnode pointers.
    Node * nodes[8];
    /// Parent node. It is 0 for root node.
    Node * parent;
    /// Index of this node in parent node children array.
    int    parentIndex;
    /// Tree pointer.
    Tree * tree;
    /// Volume rendering stuff.
    Real         baseError;
    Chunk        * volume;
    SceneNode    * sceneNode;
    /// Separate chunk parameters due to
    /// need custom baseError.
    ChunkParameters chunkParameters;
};

class Tree
{
public:
    Tree( const TreeParams & params );
    ~Tree();

    Node * newNode();
    void   removeNode( Node * node );

private:
    TreeParams   treeParams;
    SceneNode  * sceneNode;

    Node * root;
    std::list<Node *> unusedNodes;
};

}

#endif



