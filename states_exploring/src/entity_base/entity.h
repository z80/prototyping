
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
    Entity();
    virtual ~Entity();

    virtual void action( Ogre::int32 actionGroup, const Ogre::String & name, Ogre::Real value );
    //virtual Type type() = 0;

    //static template<typename T> T * cast<T>( Entity * e );

    void setSceneParent( Entity * parent, bool inheritRotation = true );
public:
    void destroySceneNode();
    Ogre::SceneNode * sceneNode;
};


}


#endif




