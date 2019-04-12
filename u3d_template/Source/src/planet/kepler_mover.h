
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

    void initKepler( Float GM, Float a, Float e, Float Omega, Float I, Float omega, Float E );
    // Computes orbit elements based on position and velocity.
    bool launch( const Vector3 & v );
    // Stop using Kepler orbit.
    void stop();
    // Obtain current position and velocity.
    Vector3 relR() const;
    Vector3 relV() const;

public:
    static const Float TIME_T;
    // Small number for
    static const Float eps;
    static const int   iters;

public:
    // Counting time.
    Float timeLow;
    Float timeHigh;

    bool active;
    Float GM;
    // Eccentricity
    Float e;
    // Semimajor axis
    Float a;
    // Semi-latus rectum.
    Float l;
    // Eccentric anomaly
    Float E;
    // Current anomaly
    Float f;
    // Period
    Float P;
    // Current time.
    Float tau;

    // Orbit unit vectors.
    Float ex[3], ey[3];
};

}

#endif



