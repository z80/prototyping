
#ifndef __SOLAR_SYSTEM_TEST_H_
#define __SOLAR_SYSTEM_TEST_H_

#include "planet_manager_base.h"

namespace Entity
{

class SolarSystemTest: public PlanetManagerBase
{
public:
    SolarSystemTest();
    ~SolarSystemTest();

    void create();
};

}


#endif




