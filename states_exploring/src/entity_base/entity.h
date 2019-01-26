
#ifndef __ENTITY_H_
#define __ENTITY_H_

#include "Ogre.h"

namespace Osp
{

/**
 * @brief The Entity class
 * This is the base class for all interacting objects.
 */
class Entity
{
public:
    enum Type { TUnspecified=0, TPart, TPlanet, TSite };
    Entity();
    virtual ~Entity();

    virtual void action( Ogre::int32 actionGroup, const Ogre::String & name, Ogre::Real value );
    Type type();

    //static template<typename T> T * cast<T>( Entity * e );

    virtual void setR( const Ogre::Vector3 & at );
    virtual Ogre::Vector3 relR() const;
    virtual void setQ( const Ogre::Quaternion & q );
    virtual Ogre::Quaternion relQ() const;
    virtual void setSceneParent( Entity * parent, bool inheritRotation = true );
    bool isParentOf( Entity * entity );

    bool relativePose( Entity * other, Ogre::Vector3 & rel_r, Ogre::Quaternion & rel_q );
public:
    void destroySceneNode();

    Ogre::SceneNode * sceneNode;
    Type              mType;
};


}


#endif




