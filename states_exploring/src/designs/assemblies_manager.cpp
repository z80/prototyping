
#include "assemblies_manager.h"
#include "state_manager.h"
#include "entity_part.h"
#include "entity_planet.h"
#include "entity_world.h"
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
    Assembly * a = new Assembly();
    a->assembliesMgr = this;
    //Ogre::SceneManager * smgr = StateManager::getSingletonPtr()->getSceneManager();
    a->sceneNode = planet->sceneNode->createChildSceneNode();
    Ogre::SceneNode * sn = a->sceneNode;
    sn->setInheritScale( false );
    sn->setInheritOrientation( true );
    sn->setPosition( at );
    sn->setOrientation( q );

    a->parent      = planet;
    a->nearSurface = true;


    // Add parts.


    // Compute center of inertia.
    a->computeCenterOfInertia();
    a->computeAssemblyRQVW();

    assemblies.push_back( a );

    return a;
}

Assembly * AssembliesManager::create( const Ogre::String & fname )
{
    Assembly * a = new Assembly();
    a->assembliesMgr = this;
    //Ogre::SceneManager * smgr = StateManager::getSingletonPtr()->getSceneManager();
    a->sceneNode = planet->sceneNode->createChildSceneNode();
    Ogre::SceneNode * sn = a->sceneNode;
    sn->setInheritScale( false );
    sn->setInheritOrientation( true );
    sn->setPosition( at );
    sn->setOrientation( q );

    a->parent      = planet;
    a->nearSurface = true;


    // Add parts.


    // Compute center of inertia.
    a->computeCenterOfInertia();
    a->computeAssemblyRQVW();

    assemblies.push_back( a );

    return a;
}

Assembly * AssembliesManager::create( const std::vector<EntityPart *> & parts,
                                      const std::vector<Connection> & connections )
{
    Assembly * a = new Assembly();
    a->assembliesMgr = this;
    //Ogre::SceneManager * smgr = StateManager::getSingletonPtr()->getSceneManager();
    a->sceneNode = planet->sceneNode->createChildSceneNode();
    Ogre::SceneNode * sn = a->sceneNode;
    sn->setInheritScale( false );
    sn->setInheritOrientation( true );
    sn->setPosition( at );
    sn->setOrientation( q );

    a->parent      = planet;
    a->nearSurface = true;


    // Add parts.
    a->parts       = parts;
    a->connections = connections;
    // Set each part assembly to be this one.
    const size_t partQty = parts.size();
    for ( size_t i=0; i<partQty; i++ )
    {
        EntityPart * p = parts[i];
        p->assembly = a;
    }

    // Compute center of inertia.
    a->computeCenterOfInertia();
    a->computeAssemblyRQVW();

    assemblies.push_back( a );

    return a;
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

void AssembliesManager::integrateDynamics( Ogre::Real time_s, int time_boost )
{
    bool needIntegration = false;
    const size_t qty = assemblies.size();
    for ( size_t i=0; i<qty; i++ )
    {
        Assembly * a = assemblies[i];
        a->integrateDynamics( time_s, time_boost );
        if ( a->forcesApplied() )
            needIntegration = true;
    }
    // All assemblies experiencing forces other than gravity
    // are moved here.
    // For those poses and velocities are supposed to be recalculated.
    if ( needIntegration )
    {
        EntityWorld * w = StateManager::getSingletonPtr()->getWorld();
        w->integrationStep( time_s, time_boost );
        for ( size_t i=0; i<qty; i++ )
        {
            Assembly * a = assemblies[i];
            if ( a->forcesApplied() )
                a->computeAssemblyRQVW();
        }
    }
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


