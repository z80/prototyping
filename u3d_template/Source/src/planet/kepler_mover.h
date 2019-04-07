
#ifndef __KEPLER_MOVER_H_
#define __KEPLER_MOVER_H_

#include <Urho3D/Urho3DAll.h>
#include "item_base.h"
#include "block.h"
#include "assembly.h"
#include "data_types.h"

using namespace Urho3D;

namespace Osp
{

class KeplerMover: public ItemBase
{
    URHO3D_OBJECT( KeplerMover, ItemBase )
public:
    KeplerMover( Context * ctx );
    ~KeplerMover();


    void Update( float dt );

    // Computes orbit elements based on position and velocity.
    bool launch(const Vector3 & v);
    // Stop using Kepler orbit.
    void stop();
    // Obtain current position and velocity.
    Vector3 relR() const;
    Vector3 relV() const;

private:
    bool active;
    Float GM;
    // Eccentricity
    Float e;
    // Semimajor axis
    Float a;
    // Inclination
    Float i;
    // Longtitude of accending node
    Float W;
    // Argument of periapsis
    Float w;
    // Current anomaly
    Float v;
};

}

#endif



