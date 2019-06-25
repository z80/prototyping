
#ifndef __CUBESPHERE_PLANET_H_
#define __CUBESPHERE_PLANET_H_

#include "cubesphere.h"
#include "planet_base.h"
#include "Urho3D/Urho3DAll.h"


namespace Osp
{

class PlanetCs: public PlanetBase, public Cubesphere::Source
{
    URHO3D_OBJECT( PlanetCs, PlanetBase )
public:
    PlanetCs( Context * ctx );
    ~PlanetCs();

    void setup( const String & json="Planets/Test.json" );

    Float dh( const Vector3d & at ) const override;
    bool  needSubdrive( const Cubesphere * s, const Face * f ) const override;

    void Start() override;
    void updateCollisions( PhysicsWorld2 * w2, Osp::WorldMover * mover, Float dist ) override;
    void initCollisions( PhysicsWorld2 * w2, Osp::WorldMover * mover, Float dist ) override;
    void finitCollisions( PhysicsWorld2 * w2 ) override;
public:
    void initParameters();
    void updateGeometry( Osp::WorldMover * mover );

    String configFileName;
    bool   initialized;
    Cubesphere::Cubesphere cubesphere;
    // Visual representation.
    CustomGeometry * cg;
    Image * heightmap,
          * colormap;
    // When dynamics is enabled
    RigidBody2      * rigidBody;
    CollisionShape2 * collisionShape;

    Vector<Cubesphere::Vertex> tris;
};


}


#endif






