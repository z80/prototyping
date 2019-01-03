
#ifndef __PLANET_MANAGER_BASE_H_
#define __PLANET_MANAGER_BASE_H_

#include "Ogre.h"

namespace Entity
{

class EntityPlanet;

class PlanetManagerBase
{
public:
    PlanetManagerBase();
    virtual ~PlanetManagerBase();

    /// Need to create all the planets here.
    void create();

    /// Each planet has it's movement parameters.
    /// This method executes planet movement and
    /// updates poses and velocities.
    void integrateKinematics( Ogre::Real t_sec, int timeBoost=1 );

    int planetsQty() const;
    EntityPlanet * planet( int index );
    EntityPlanet * planet( const Ogre::String & name );

public:
    virtual void createImpl() = 0;
    void destroy();

    typedef Ogre::shared_ptr<EntityPlanet> EntityPlanetPtr;
    std::vector<EntityPlanet *> planets;
};



}






#endif



