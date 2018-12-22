
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
    void setR( const Ogre::Vector3 & at );
    void setQ( const Ogre::Quaternion & q );

    void integrateKinematics( Ogre::Real t_sec );
public:
    /// Absolute position.
    Ogre::Vector3 absoluteR() const;
    /// Translational velocity caused by all movements.
    Ogre::Vector3 absoluteV() const;
    /// Angular velocity.
    Ogre::Vector3 absoluteW() const;
    /// Velocity caused by self rotation.
    Ogre::Vector3 rotV( const Ogre::Vector3 & at, bool local=false ) const;
    /// Orientation.
    Ogre::Quaternion absoluteQ() const;


    Ogre::Entity     * visualEntity;
    Ogre::SceneNode  * sceneNode;
    btRigidBody      * rigidBody;
    btCollisionShape * collisionShape;
    BtOgre::RigidBodyState * bodyState;

    AirMesh::AtmosphereForces atm;
    AirMesh::Gravity          g;

    /// Parent planet or null pointer.
    EntityPlanet * parent;
    // Planet movement.

    // Self rotation.
    /// Self rotation angular velocity.
    Ogre::Real       rotPeriod;
    /// Initial transformation.
    Ogre::Quaternion rotQuat;
    /// Self rotation phase.
    Ogre::Real       rotTime;

    // Orbiting.
    /// Orbit radius.
    Ogre::Real orbitRadius;
    /// Orbit angular velocity.
    Ogre::Real orbitPeriod;
    /// Orbit initial transformation.
    Ogre::Quaternion orbitQuat;
    /// Orbit phase.
    Ogre::Real       orbitTime;
};


}

#endif


