
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


class EntityPlanet: public Entity
{
public:
    EntityPlanet();
    ~EntityPlanet();

    void addForces( EntityPart & part );
    void setPosition( const Ogre::Vector3 & at );
    void setRotation( const Ogre::Quaternion & q );

public:
    Ogre::Entity     * visualEntity;
    Ogre::SceneNode  * sceneNode;
    btRigidBody      * rigidBody;
    btCollisionShape * collisionShape;
    BtOgre::RigidBodyState * bodyState;

    AirMesh::AtmosphereForces atm;
    AirMesh::Gravity          g;
};


}

#endif


