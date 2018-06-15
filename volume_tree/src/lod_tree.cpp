
#include "lod_tree.h"

namespace LodTree
{

Node::Node()
{
    volume = 0;
    sceneNode = 0;
}

Node::~Node()
{
    if ( sceneNode )
    {
        sceneNode->detachAllObjects();
        sceneNode = 0;
    }
    if ( volume )
    {
        OGRE_DELETE volume;
        volume = 0;
    }
}

Node::Node( const Node & inst )
{
    *this = inst;
}

const Node & Node::operator=( const Node & inst )
{
    if ( this != &inst )
    {
        at       = inst.at;
        halfSz   = inst.halfSz;
        level    = inst.level;
        leafNode = inst.leafNode;
        for ( int i=0; i<8; i++ )
            nodes[i] = inst.nodes[i];
        tree      = inst.tree;
        volume    = inst.volume;
        sceneNode = inst.sceneNode;

    }
    return *this;
}

bool Node::needSubdriven( SceneNode * camNode ) const
{
    if ( level >= tree->params.maxLevel )
        return false;
    static const Real k = 1.4 + 2.0;
    const Vector3 camAt  = camNode->convertLocalToWorldPosition( Vector3() );
    const Vector3 nodeAt = tree->sceneNode->convertLocalToWorldPosition( at );
    const Real d = camAt.distance( nodeAt );
    const Real minD = k * halfSz;
    if ( d > minD )
        return false;
    return true;
}

bool Node::subdrive()
{

}

}

