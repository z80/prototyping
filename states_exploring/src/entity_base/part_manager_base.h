
#ifndef __PART_MANAGER_BASE_H_
#define __PART_MANAGER_BASE_H_

#include "entity.h"

namespace Entity
{

class EntityPart;

class PartManagerBase
{
public:
    PartManagerBase();
    virtual ~PartManagerBase();

    /**
     * @brief entityPart
     * @param name
     * @param r
     * @param q
     * @param parentNode
     * @return
     * Here parentNode is the one relative to
     * what "r" and "q" are provided.
     */
    virtual EntityPart * create( const std::string & name,
                                 const Ogre::Vector3 & r = Ogre::Vector3::ZERO,
                                 const Ogre::Quaternion & q = Ogre::Quaternion::IDENTITY,
                                 Entity * parent = 0 ) = 0;
    void remove( EntityPart * part );
private:
    void destroyAllParts();
    std::vector<EntityPart *> parts;
};






}






#endif

