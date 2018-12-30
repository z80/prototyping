
#include "planet_manager_base.h"
#include "entity_planet.h"

namespace Entity
{

PlanetManagerBase::PlanetManagerBase()
{
    create();
}

PlanetManagerBase::~PlanetManagerBase()
{
    destroy();
}

void PlanetManagerBase::create()
{

}

void PlanetManagerBase::integrateKinematics( Ogre::Real t_sec )
{
    for ( std::vector<EntityPlanetPtr>::iterator it=planets.begin();
          it!=planets.end(); it++ )
    {
        EntityPlanetPtr & p = *it;
        p->integrateKinematics( t_sec );
    }
}

int PlanetManagerBase::planetsQty() const
{
    const int qty = static_cast<int>( planets.size() );
    return qty;
}

EntityPlanet * PlanetManagerBase::planet( int index )
{
    const int qty = static_cast<int>( planets.size() );
    if ( index >= qty )
        return 0;
    EntityPlanet * p = planets[index].get();
    return p;
}

void PlanetManagerBase::destroy()
{
    planets.clear();
}


}




