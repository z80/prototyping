
#ifndef __SOLAR_SYSTEM_TEST_H_
#define __SOLAR_SYSTEM_TEST_H_

#include "planet_manager_base.h"

namespace Osp
{

class PlanetManagerTest: public PlanetManagerBase
{
public:
    PlanetManagerTest();
    ~PlanetManagerTest();

public:
    void createImpl();
};

}


#endif




