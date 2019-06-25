
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

    void Start() override;
    void updateCollisions( PhysicsWorld2 * w2, Osp::WorldMover * mover, Float dist );
    void initCollisions( PhysicsWorld2 * w2, Osp::WorldMover, Float dist );
    void finitCollisions( PhysicsWorld2 * w2 );
public:
    // Visual rrepresentation.
    CustomGeometry * cg;
    // When dynamics is enabled
    RigidBody2      * rigidBody;
    CollisionShape2 * collisionShape;
};


}


#endif






