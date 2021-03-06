
#include "entity.h"
#include "state_manager.h"
#include "name_generator.h"
#include "OgreSceneManager.h"

namespace Osp
{

Entity::Entity()
{
    Ogre::SceneManager * smgr = StateManager::getSingletonPtr()->getSceneManager();
    sceneNode = smgr->getRootSceneNode()->createChildSceneNode( NameGenerator::Next( "Entity" ) );
    sceneNode->setInheritOrientation( true );
    sceneNode->setInheritScale( false );

    Ogre::UserObjectBindings & uob = sceneNode->getUserObjectBindings();
    uob.setUserAny( Ogre::Any( this ) );

    mType     = TUnspecified;
}

Entity::~Entity()
{
    destroySceneNode();
}

void Entity::action( Ogre::int32 actionGroup, const Ogre::String & name, Ogre::Real value )
{

}

Entity::Type Entity::type()
{
    return mType;
}

void Entity::setR( const Ogre::Vector3 & at )
{
    sceneNode->setPosition( at );
}

Ogre::Vector3 Entity::relR() const
{
    return sceneNode->getPosition();
}

void Entity::setQ( const Ogre::Quaternion & q )
{
    sceneNode->setOrientation( q );
}

Ogre::Quaternion Entity::relQ() const
{
    return sceneNode->getOrientation();
}

void Entity::setSceneParent( Entity * parent, bool inheritRotation )
{
    Ogre::Vector3    rel_r;
    Ogre::Quaternion rel_q;
    const bool relPoseOk = relativePose( parent, rel_r, rel_q );
    if ( !sceneNode )
    {
        Ogre::SceneManager * smgr = StateManager::getSingletonPtr()->getSceneManager();
        sceneNode = smgr->getRootSceneNode()->createChildSceneNode();
        sceneNode->setInheritScale( false );
    }

    sceneNode->getParentSceneNode()->removeChild( sceneNode );
    if ( (parent != 0) && (parent->sceneNode != 0) )
        parent->sceneNode->addChild( sceneNode );
    else
    {
        Ogre::SceneManager * smgr = StateManager::getSingletonPtr()->getSceneManager();
        smgr->getRootSceneNode()->addChild( sceneNode );
    }

    sceneNode->setInheritOrientation( inheritRotation );
    if ( relPoseOk )
    {
        sceneNode->setPosition( rel_r );
        sceneNode->setOrientation( rel_q );
    }
}

bool Entity::isParentOf( Entity * entity ) const
{
    Ogre::SceneManager * smgr = StateManager::getSingletonPtr()->getSceneManager();
    const Ogre::SceneNode * root = smgr->getRootSceneNode();
    Ogre::SceneNode * node = entity->sceneNode;
    while ( ( node != root ) && ( node != 0 ) )
    {
        if ( sceneNode == node )
            return true;
        node = node->getParentSceneNode();
    }

    return false;
}

Entity * Entity::parentEntity() const
{
    Ogre::SceneNode * n = sceneNode->getParentSceneNode();
    if ( !n )
        return 0;

    Ogre::UserObjectBindings & uob = n->getUserObjectBindings();
    const Ogre::Any & a = uob.getUserAny();
    if ( !a.has_value() )
        return 0;

    const Osp::Entity * centity = Ogre::any_cast<Osp::Entity *>( a );
    if ( !centity )
        return 0;

    Osp::Entity * entity = const_cast<Osp::Entity *>( centity );
    return entity;
}

bool Entity::relativePose( Entity * other, Ogre::Vector3 & rel_r, Ogre::Quaternion & rel_q )
{
    // root->a->b->c->d->e->this
    // root->a->b->other->f->g
    // The idea is to accumulate relative position and orientation.
    Ogre::Vector3    r = Ogre::Vector3::ZERO;
    Ogre::Quaternion q = Ogre::Quaternion::IDENTITY;

    Ogre::SceneManager * smgr = StateManager::getSingletonPtr()->getSceneManager();
    const Ogre::SceneNode * root = smgr->getRootSceneNode();

    // Get all ancestors of current node.
    // Make it static as graphics is in one thread.
    static std::vector<Ogre::SceneNode *> allAncestorsA;
    allAncestorsA.clear();
    Ogre::SceneNode * nodeA = this->sceneNode;
    do {
        allAncestorsA.push_back( nodeA );
        nodeA = nodeA->getParentSceneNode();
    } while ( nodeA != 0 );
    const size_t allQtyA = allAncestorsA.size();

    Ogre::SceneNode * nodeB = other->sceneNode;
    static std::vector<Ogre::SceneNode *> ancestorsB;
    ancestorsB.clear();
    size_t indA = allQtyA;
    do {
        ancestorsB.push_back( nodeB );
        // Check if nodeB is in allAncestorsA.
        for ( size_t i=0; i<allQtyA; i++ )
        {
            nodeA = allAncestorsA[i];
            if ( nodeA == nodeB )
            {
                indA = i;
                break;
            }
        }
        if ( indA != allQtyA )
            break;
        // Get parent.
        nodeB = nodeB->getParentSceneNode();
    } while ( nodeB != 0 );

    // If reached the root and didn't meed 
    // anything common just break.
    if ( indA == allQtyA )
        return false;

    // Here there is a closest common ancestor.
    // First find pose of nodeA in it's ref. frame.
    Ogre::Vector3    ra = Ogre::Vector3::ZERO;
    Ogre::Quaternion qa = Ogre::Quaternion::IDENTITY;
    for ( size_t i=0; i<indA; i++ )
    {
        nodeA = allAncestorsA[i];
        const Ogre::Quaternion q = nodeA->getOrientation();
        const Ogre::Vector3    r = nodeA->getPosition();
        ra = q*ra;
        ra = r + ra;
        qa = q * qa;
    }

    Ogre::Vector3    rb = Ogre::Vector3::ZERO;
    Ogre::Quaternion qb = Ogre::Quaternion::IDENTITY;
    const size_t indB = ancestorsB.size()-1;
    for ( size_t i=0; i<indB; i++ )
    {
        Ogre::SceneNode * nodeB = ancestorsB[i];
        const Ogre::Quaternion q = nodeB->getOrientation();
        const Ogre::Vector3    r = nodeB->getPosition();
        rb = q*rb;
        rb = r + rb;
        qb = q * qb;
    }

    rel_r = ra - rb;
    // This might be wrong. 
    // I probably don't need quaternion at all.
    rel_q = qb.Inverse() * qa;
    return true;
}

void Entity::destroySceneNode()
{
    if ( !sceneNode )
        return;
    Ogre::SceneManager * smgr = StateManager::getSingletonPtr()->getSceneManager();
    if ( !smgr->hasSceneNode( sceneNode->getName() ) )
        return;
    std::vector<Ogre::Node *> nodes;
    Ogre::Node::ChildNodeIterator it = sceneNode->getChildIterator();
    while ( it.hasMoreElements() )
    {
        Ogre::Node * n = it.getNext();
        nodes.push_back( n );
    }
    for ( std::vector<Ogre::Node *>::iterator it=nodes.begin();
          it!=nodes.end(); it++ )
    {
        Ogre::Node * n = *it;
        sceneNode->removeChild( n );
        smgr->getRootSceneNode()->addChild( n );
    }
    smgr->destroySceneNode( sceneNode );
    sceneNode = 0;
}




}



