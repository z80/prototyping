
#ifndef __LAUNCH_SITE_H_
#define __LAUNCH_SITE_H_

#include "Ogre.h"

namespace Entity
{

class EntityPlanet;

/**
 * @brief The Site class
 * Vehicle assembly building, runway, launch pad, etc.
 * So far it is incomplete. But will add content later on.
 */
class Site
{
public:
    Site();
    virtual ~Site();

    /// Site name.
    Ogre::String      name;
    /// Function "assembly", "launch_pod".
    Ogre::String      function;
    Ogre::SceneNode * sceneNode;
    Ogre::Entity    * entity;
    EntityPlanet    * planet;

    /// Position on the planet.
    Ogre::Vector3    r;
    /// Orientation on the planet.
    Ogre::Quaternion q;
};


}

#endif



