
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
                              const btVector3 & velocity,
                              btVector3 & f, btVector3 & p ) const;
    virtual btScalar density( const btVector3 & at ) const;
    btScalar viscosityFwd,
             viscosityBwd,
             viscosityLat,
             radius, height, groundDensity;
    btVector3 r0;
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

    const btVector3 & forceTorque( const AtmosphereForces & f,
                                   const btVector3 & position,
                                   const btVector3 & velocity,
                                   const btQuaternion & orientation,
                                   btVector3 & resF, btVector3 & resP ) const;

public:
    std::list<Triangle> tris;
};


}

#endif





