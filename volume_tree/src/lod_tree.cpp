
#include "lod_tree.h"

namespace LodTree
{

Node::Node()
{
    tree = 0;
    volume = 0;
    sceneNode = 0;
    for ( int i=0; i<8; i++ )
        nodes[i] = 0;
    parent      = 0;
    parentIndex = -1;
    leafNode = true;
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
        parent   = inst.parent;
        parentIndex = inst.parentIndex;

        tree      = inst.tree;

        baseError = inst.baseError;
        volume    = inst.volume;
        sceneNode = inst.sceneNode;
        chunkParameters = inst.chunkParameters;

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

bool Node::subdrive( SceneNode * camNode )
{
    // If level is too big.
    const int newLevel = level+1;
    if ( newLevel > tree->treeParams.maxLevel )
    {
        leafNode = true;
        return true;
    }
    // If not close enough to the camera so subdriving
    // makes no difference.
    const bool needToBeSubdriven = needSubdriven( camNode );
    if ( !needToBeSubdriven )
    {
        leafNode = true;
        return true;
    }
    // If has no geometry inside.
    const bool hasGeometry = this->hasGeometry();
    if ( !hasGeometry )
    {
        leafNode = true;
        return true;
    }

    leafNode = false;
    // Has geometry, need to be subtriven and level allows that.
    // Subdriving the node.

    const Real sz  = halfSz * 0.5;
    const Real err = baseError * 0.5;
    static const int s[8][3] =
    {
        {-1, -1, -1},
        { 1, -1, -1},
        {-1,  1, -1},
        { 1,  1, -1},
        {-1, -1,  1},
        { 1, -1,  1},
        {-1,  1,  1},
        { 1,  1,  1},
    };

    for ( int i=0; i<8; i++ )
    {
        Node * node = tree->newNode();
        if ( !node )
            return false;
        // Node position and dimensions.
        node->baseError = err;
        node->halfSz    = sz;
        node->at.x = at.x+s[i][0]*sz;
        node->at.y = at.y+s[i][1]*sz;
        node->at.z = at.z+s[i][2]*sz;

        // Node parent information.
        node->parent = this;
        node->parentIndex = i;
        nodes[i] = node;

        // Subdrive recursively.
        const bool ok = node->subdrive( camNode );
        if ( !ok )
            return false;
    }

    return true;
}

bool Node::hasGeometry() const
{
    Source * src = tree->treeParams.src;
    // Don't split if nothing is inside.
    Real centerValue = src->getValue( at );
    if ( Math::Abs(centerValue) > (to - from).length() * src->getVolumeSpaceToWorldSpaceFactor() )
        return false;
    return true;
}












Tree::Tree( const TreeParams & params )
{
    treeParams = params;
    root = 0;
    sceneNode = treeParams.sceneManager->getRootSceneNode()->createChildSceneNode();
}

Tree::~Tree()
{

}

Node * Tree::newNode()
{
    if ( !unusedNodes.empty() )
    {
        Node * node = unusedNodes.front();
        unusedNodes.pop_front();
        return node;
    }
    Node * node = new Node();
    node->tree      = this;
    node->volume    = 0;
    node->sceneNode = 0;
}

void Tree::removeNode( Node * node )
{
    // First take care of child nodes.
    for ( int i=0; i<8; i++ )
    {
        Node * node = nodes[i];
        if ( node )
        {
            removeNode( node );
            nodes[i] = 0;
        }
    }
    unusedNodes.push_back( node );
}





}

