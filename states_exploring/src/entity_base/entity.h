
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
};


}


#endif




