
#include "planet_manager_base.h"
#include "entity_planet.h"

namespace Entity
{

PlanetManagerBase::PlanetManagerBase()
{
}

PlanetManagerBase::~PlanetManagerBase()
{
    destroy();
}

void PlanetManagerBase::create()
{
    if ( planets.empty() )
        createImpl();
}

void PlanetManagerBase::integrateKinematics( Ogre::Real t_sec, int timeBoost )
{
    const size_t qty = planets.size();
    for ( size_t i=0; i<qty; i++ )
    {
        EntityPlanet * p = planets[i];
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
    EntityPlanet * p = planets[index];
    return p;
}

EntityPlanet * PlanetManagerBase::planet( const Ogre::String & name )
{
    const size_t qty = planets.size();
    for ( size_t i=0; i<qty; i++ )
    {
        EntityPlanet * p = planets[i];
        const Ogre::String & n = p->name();
        if ( name == n )
            return p;
    }

    return 0;
}

void PlanetManagerBase::destroy()
{
    const size_t qty = planets.size();
    for ( size_t i=0; i<qty; i++ )
    {
        EntityPlanet * p = planets[i];
        delete p;
    }
    planets.clear();
}


}




