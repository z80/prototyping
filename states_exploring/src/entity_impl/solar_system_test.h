
#ifndef __SOLAR_SYSTEM_TEST_H_
#define __SOLAR_SYSTEM_TEST_H_

#include "entity.h"
#include "entity_planet.h"
#include "entity_test_planet.h"

namespace Entity
{

class SolarSystemTest
{
public:
    SolarSystemTest();
    ~SolarSystemTest();

    void integrateKinematics( Ogre::Real t_sec );

    void create();
    void destroy();


public:
    typedef Ogre::shared_ptr<EntityTestPlanet> EntityTestPlanetPtr;
    std::vector<EntityTestPlanetPtr> planets;
};

}


#endif




