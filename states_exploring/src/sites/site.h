
#ifndef __SITE_H_
#define __SITE_H_

#include "Ogre.h"
#include "entity.h"

class btRigidBody;
class btCollisionShape;


namespace Osp
{

class EntityPlanet;

/**
 * @brief The Site class
 * Vehicle assembly building, runway, launch pad, etc.
 * So far it is incomplete. But will add content later on.
 */
class Site: public Entity
{
public:
    Site();
    virtual ~Site();

    /// Site name.
    Ogre::String      name;
    /// Function "assembly", "launch_pod".
    Ogre::String      function;

    /// There should be at least one visual entity, right?
    Ogre::Entity     * visualEntity;
    btRigidBody      * rigidBody;
    btCollisionShape * collisionShape;
};


}

#endif



