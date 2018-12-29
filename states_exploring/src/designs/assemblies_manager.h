
#ifndef __ASSEMBLIES_MANAGER_H_
#define __ASSEMBLIES_MANAGER_H_

#include "Ogre.h"

namespace Config
{
    class ConfigReader;
}

namespace Entity
{

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

public:
    std::vector<Assembly *> assemblies;
};


}


#endif




