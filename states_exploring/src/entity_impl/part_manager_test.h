
#ifndef __PART_MANAGER_TEST_H_
#define __PART_MANAGER_TEST_H_

#include "entity.h"
#include "part_manager_base.h"

namespace Entity
{

class PartManagerTest: public PartManagerBase
{
public:
    PartManagerTest();
    ~PartManagerTest();

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
    virtual EntityPart * create( const std::string & name,
                                 const Ogre::Vector3 & r = Ogre::Vector3::ZERO,
                                 const Ogre::Quaternion & q = Ogre::Quaternion::IDENTITY );


private:
    class PD;
    PD * pd;
};






}






#endif


