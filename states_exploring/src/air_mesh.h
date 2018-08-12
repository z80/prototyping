
#ifndef __AIR_MESH_H_
#define __AIR_MESH_H_

#include "Ogre.h"
#include "btBulletDynamicsCommon.h"

namespace AirMesh
{

struct Triangle
{
    btScalar  area;
    btVector3 at;
    btVector3 norm;
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
};


class AirMesh
{
public:
    static bool airMesh( Ogre::Entity * e, AirMesh & a );

    AirMesh();
    ~AirMesh();
    AirMesh( const AirMesh & inst );
    const AirMesh & operator=( const AirMesh & inst );

    const btVector3 & forceTorque( btScalar density, const AtmosphereForces & f,
                                   const btVector3 & velocity,
                                   const btQuaternion & orientation,
                                   btVector3 & resF, btVector3 & resP ) const;

public:
    std::list<Triangle> tris;
};


}

#endif





