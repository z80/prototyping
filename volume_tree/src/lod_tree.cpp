
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
    level = 0;
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
    if ( level >= tree->treeParams.maxLevel )
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
        node->level = newLevel;
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
    const Vector3 from( at - halfSz );
    const Vector3 to( at + halfSz );
    if ( Math::Abs(centerValue) > (to - from).length() * src->getVolumeSpaceToWorldSpaceFactor() )
        return false;
    return true;
}

bool Node::createVolume()
{
    if ( leafNode )
    {
        if ( !hasGeometry() )
            return true;

        destroyVolume();
        volume = OGRE_NEW Chunk();

        const Vector3 from( at - this->halfSz );
        const Vector3 to(   at + this->halfSz );
        chunkParameters = tree->treeParams;
        chunkParameters.baseError = this->baseError;
        volume->load( tree->sceneNode, from, to, 0, &chunkParameters );
    }
    else
    {
        for ( int i=0; i<8; i++ )
        {
            Node * node = nodes[i];
            if ( !node )
                continue;
            const bool volumeOk = node->createVolume();
            if ( !volumeOk )
                return false;
        }
    }
    return true;
}

void Node::destroyVolume()
{
    if ( volume )
    {
        OGRE_DELETE volume;
        volume = 0;
    }
}

void Node::setMaterial( MaterialPtr m )
{
    if ( leafNode )
    {
        if ( !volume )
            return;
        volume->setMaterial( m );
    }
    else
    {
        for ( int i=0; i<8; i++ )
        {
            Node * node = nodes[i];
            if ( !node )
                continue;
            node->setMaterial( m );
        }
    }
}












Tree::Tree( const TreeParams & params )
{
    treeParams = params;
    root = 0;
    sceneNode = treeParams.sceneManager->getRootSceneNode()->createChildSceneNode();
}

Tree::~Tree()
{
    deleteAllNodes();
}

bool Tree:: buildTree( SceneNode * camNode )
{
    removeNode( root );
    root = newNode();
    root->at     = treeParams.at;
    root->halfSz = treeParams.halfSz;
    root->level  = 1;
    root->parent = 0;
    root->parentIndex = -1;
    const bool subdriveOk = root->subdrive( camNode );
    if ( !subdriveOk )
        return false;
    const bool volumesOk = root->createVolume();
    if ( !volumesOk )
        return false;
    return true;
}

void Tree::setMaterial( MaterialPtr m )
{
    if ( root )
        root->setMaterial( m );
}

bool Tree::buildVolumes()
{
    if ( !root )
        return false;

    const bool res = root->createVolume();
    return res;
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
    if ( !node )
        return;
    // First take care of child nodes.
    for ( int i=0; i<8; i++ )
    {
        Node * n = node->nodes[i];
        if ( n )
        {
            removeNode( n );
            node->nodes[i] = 0;
        }
    }
    // If node has a parent clear info about in
    // parent structures.
    if ( node->parent )
    {
        Node * p = node->parent;
        p->nodes[node->parentIndex] = 0;
        node->parent = 0;
        node->parentIndex = -1;
    }
    // Place node to reserve.
    unusedNodes.push_back( node );

    // Delete nodes volume.
    if ( node->volume )
    {
        OGRE_DELETE node->volume;
        node->volume = 0;
    }
}

void Tree::deleteAllNodes()
{
    removeNode( root );
    for ( std::list<Node *>::iterator it=unusedNodes.begin();
          it!=unusedNodes.end(); it++ )
    {
        Node * node = *it;
        delete node;
    }
    unusedNodes.clear();
}





}

