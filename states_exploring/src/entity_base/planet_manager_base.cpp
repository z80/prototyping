
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

void PlanetManagerBase::integrateKinematics( Ogre::Real t_sec, int timeBoost )
{
    for ( std::vector<EntityPlanetPtr>::iterator it=planets.begin();
          it!=planets.end(); it++ )
    {
        EntityPlanetPtr & p = *it;
        p->integrateKinematics( t_sec, timeBoost );
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

EntityPlanet * PlanetManagerBase::planet( const Ogre::String & name )
{
    const size_t qty = planets.size();
    for ( size_t i=0; i<qty; i++ )
    {
        EntityPlanet * p = planets[i].get();
        const Ogre::String & n = p->name();
        if ( name == n )
            return p;
    }

    return 0;
}

void PlanetManagerBase::destroy()
{
    planets.clear();
}


}




