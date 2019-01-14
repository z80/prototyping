
#ifndef __ENTITY_H_
#define __ENTITY_H_

#include "Ogre.h"

namespace Entity
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

    void setSceneParent( Entity * parent, bool inheritRotation = true );

    void relativePose( Entity * other, Ogre::Vector3 & r, Ogre::Quaternion & q );
public:
    void destroySceneNode();

    Ogre::SceneNode * sceneNode;
    Type              mType;
};


}


#endif




