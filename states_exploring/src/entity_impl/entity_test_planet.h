
#ifndef __ENTITY_TEST_PLANET_H_
#define __ENTITY_TEST_PLANET_H_

#include "entity_planet.h"


namespace Entity
{

class EntityTestPlanet: public EntityPlanet
{
public:
    EntityTestPlanet();
    virtual ~EntityTestPlanet();

    void create();
    void update();

public:
    Ogre::Real R;
};

}

#endif

