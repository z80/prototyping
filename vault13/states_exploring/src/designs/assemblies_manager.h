
#ifndef __ASSEMBLIES_MANAGER_H_
#define __ASSEMBLIES_MANAGER_H_

#include "Ogre.h"
#include "design.h"

namespace Config
{
    class ConfigReader;
}

namespace Osp
{

class EntityPlanet;
class Assembly;

class AssembliesManager
{
public:
    AssembliesManager();
    ~AssembliesManager();

    /// Load AssembliesManager's parameters from a config file.
    bool load( Config::ConfigReader * cr );
    /// Manually set parent, origin and orientation of assemblies to be created.
    void setOrigin( EntityPlanet * planet,
                    const Ogre::Vector3 & at,
                    const Ogre::Quaternion & q=Ogre::Quaternion::IDENTITY );

    /// Ways to create new assemblies.
    Assembly * create( const Design & design );
    Assembly * create( const Ogre::String & fname );
    Assembly * create( const std::vector<Block *> & parts,
                       const std::vector<BlockConnection *> & connections );
    /// Removes an assembly. Mostly it removes a pointer from the array.
    void destroy( Assembly * assembly );

    /// Number of designs.
    int designsQty() const;
    /// Get a design by it's index.
    Design * design( int ind );

    /// Dynamics integration.
    /// It is supposed to call dynamics world integration in
    /// the case if there is at least one assembly near surface.
    void integrateDynamics( Ogre::Real time_s, int time_boost=1 );
public:
    void cleanup();

    /// Creation origin.
    EntityPlanet   * planet;
    Ogre::Vector3    at;
    Ogre::Quaternion q;
    /// List of existing assemblies.
    std::vector<Assembly *> assemblies;
    /// List of available desings.
    std::vector<Design>     designs;
};


}


#endif




