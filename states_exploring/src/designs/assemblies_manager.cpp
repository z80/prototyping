
#include "assemblies_manager.h"
#include "entity_part.h"
#include "entity_planet.h"
#include "assembly.h"

namespace Entity
{

AssembliesManager::AssembliesManager()
{
    planet = 0;
}

AssembliesManager::~AssembliesManager()
{

}

bool AssembliesManager::load( Config::ConfigReader * cr )
{

}

void AssembliesManager::setOrigin( EntityPlanet * planet,
                                   const Ogre::Vector3 & at,
                                   const Ogre::Quaternion & q )
{

}

Assembly * AssembliesManager::create( const Design & design )
{

}

Assembly * AssembliesManager::create( const Ogre::String & fname )
{

}

Assembly * AssembliesManager::create( const std::vector<EntityPart *> & parts,
                                      const std::vector<Connection> & connections )
{
    Assembly * a = new Assembly();
    a->assembliesMgr = this;
}

void AssembliesManager::remove( Assembly * assembly )
{
    const size_t qty = assemblies.size();
    for ( size_t i=0; i<qty; i++ )
    {
        Assembly * a = assemblies[i];
        if ( a == assembly )
        {
            if ( i<(qty-1) )
                assemblies[i] = assemblies[qty-1];
            assemblies.resize( qty-1 );
        }
    }
}

int AssembliesManager::designsQty() const
{
    const int qty = static_cast<int>( designs.size() );
}

Design * AssembliesManager::design( int ind )
{
    const int qty = static_cast<int>( designs.size() );
    Design * d = ( ind < qty ) ? &(designs[ind]) : 0;
    return d;
}

void AssembliesManager::cleanup()
{
    const size_t qty = assemblies.size();
    for ( size_t i=0; i<qty; i++ )
    {
        Assembly * a = assemblies[i];
        delete a;
    }
    // The array is supposed to be empty at this point as
    // on deletion assemblies are supposed to remove themself
    // from the array.
}


}


