
#ifndef __PLANET_FORCES_H_
#define __PLANET_FORCES_H_

#include <Urho3D/Urho3DAll.h>
#include "item_base.h"
#include "data_types.h"

namespace Osp
{

class Block;

class PlanetForces: public LogicComponent
{
    URHO3D_OBJECT( PlanetForces, LogicComponent )
public:
    PlanetForces( Context * ctx );
    ~PlanetForces();

    virtual void Start() override;
    virtual void Update( float dt ) override;

    void applyClose( Block * b );
    void applyFar( Block * b );

public:
    virtual void applyGravity( Block * b );
    void clearGravity(Block * b );
    virtual void applyFriction( Block * b );

    bool  atmosphereParams( const Vector3d &at, Float & density, Float & viscosity, Float & temperature );

    SharedPtr<ItemBase> planet;

    // Gravity parameter.
    Float GM_;
    // Gravity radius.
    Float R_;

    // Air friction parameters.
    // Atmosphere height.
    Float atmHeight_;
    // Ground level density.
    Float density_;
    // Viscosity.
    Float viscosity_;

    // Temperature parameters.
    Float groundT_;
    Float highAltitudeT_;

};

}

#endif


