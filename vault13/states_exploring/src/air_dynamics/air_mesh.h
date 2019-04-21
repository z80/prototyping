
#ifndef __AIR_MESH_H_
#define __AIR_MESH_H_

#include "Ogre.h"
#include "btBulletDynamicsCommon.h"

namespace AirMesh
{

struct Triangle
{
    Ogre::Real    area;
    Ogre::Vector3 at;
    Ogre::Vector3 norm;
};

class AtmosphereForces
{
public:
    AtmosphereForces();
    virtual ~AtmosphereForces();

    virtual void forceTorque( const Triangle & tri,
                              const Ogre::Vector3 & velocity,
                              Ogre::Vector3 & f, Ogre::Vector3 & p ) const;
    virtual Ogre::Real density( const Ogre::Vector3 & at ) const;
    Ogre::Real viscosityFwd,
               viscosityBwd,
               viscosityLat,
               radius, height, groundDensity;
    Ogre::Vector3 r0;
};

class Gravity
{
public:
    Gravity();
    ~Gravity();

    virtual Ogre::Vector3 gravity( const Ogre::Real m, const Ogre::Vector3 & r );
    virtual Ogre::Vector3 gravity( const Ogre::Real m, const Ogre::Vector3 & r0, const Ogre::Vector3 & r );

    Ogre::Real  GM;
    Ogre::Real  radius;
};


class AirMesh
{
public:
    static bool airMesh( Ogre::Entity * e, AirMesh & a );
    static bool airMesh( Ogre::MeshPtr m, AirMesh & a );

    AirMesh();
    ~AirMesh();
    AirMesh( const AirMesh & inst );
    const AirMesh & operator=( const AirMesh & inst );

    const Ogre::Vector3 & forceTorque( const AtmosphereForces & f,
                                       const Ogre::Vector3 & position,
                                       const Ogre::Vector3 & velocity,
                                       const Ogre::Quaternion & orientation,
                                       Ogre::Vector3 & resF, Ogre::Vector3 & resP ) const;

public:
    std::vector<Triangle> tris;
};


}

#endif





