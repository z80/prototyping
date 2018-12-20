
#ifndef __ENTITY_FACTORY_H_
#define __ENTITY_FACTORY_H_

#include "entity.h"

namespace Entity
{

class EntityFactory: public Ogre::Singleton<EntityFactory>
{
public:
    EntityFactory();
    ~EntityFactory();

    /**
     * @brief entityPart
     * @param name
     * @param at
     * @param q
     * @param parentNode
     * @return
     * Here parentNode is the one relative to
     * what at and q are provided.
     */
    Entity * create( const std::string & name,
                     const Ogre::Vector3 & at = Ogre::Vector3(0.0, 0.0, 0.0),
                     const Ogre::Quaternion & q = Ogre::Quaternion( 1.0, 0.0, 0.0, 0.0 ),
                     Ogre::SceneNode * parentNode = 0 );

private:
    class PD;
    PD * pd;
};






}






#endif


