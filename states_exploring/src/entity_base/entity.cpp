
#include "entity.h"
#include "state_manager.h"
#include "name_generator.h"
#include "OgreSceneManager.h"

namespace Entity
{

Entity::Entity()
{
    Ogre::SceneManager * smgr = StateManager::getSingletonPtr()->getSceneManager();
    sceneNode = smgr->getRootSceneNode()->createChildSceneNode( NameGenerator::Next( "Entity" ) );
    sceneNode->setInheritOrientation( true );
    sceneNode->setInheritScale( false );

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

void Entity::setSceneParent( Entity * parent, bool inheritRotation )
{
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
}

void Entity::relativePose( Entity * other, Ogre::Vector3 & r, Ogre::Quaternion & q )
{
    // root->a->b->c->d->e->this
    // root->a->b->other->f->g
    // The idea is to accumulate relative position and orientation.
    Ogre::Vector3    r = Ogre::Vector3::ZERO;
    Ogre::Quaternion q = Ogre::Quaternion::IDENTITY;

    Ogre::SceneManager * smgr = StateManager::getSingletonPtr()->getSceneManager();
    const Ogre::SceneNode * root = smgr->getRootSceneNode();
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



