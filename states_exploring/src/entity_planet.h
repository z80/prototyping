
#ifndef __PLANET_H_
#define __PLANET_H_

#include "Ogre.h"
#include "entity.h"
#include "air_mesh.h"
#include "entity_part.h"

namespace Config
{
    class ConfigReader;
}

namespace Entity
{


class EntityPlanet
{
public:
    EntityPlanet();
    ~EntityPlanet();

    void addAirFriction( EntityPart & part );

public:
    Ogre::Entity     * visualEntity;
    Ogre::SceneNode  * sceneNode;
    btRigidBody      * rigidBody;
    btCollisionShape * collisionShape;
    BtOgre::RigidBodyState * bodyState;

};


}

#endif


