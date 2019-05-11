
#include "planet_forces.h"
#include "block.h"
#include "planet_base.h"

namespace Osp
{

PlanetForces::PlanetForces( Context * ctx )
    : LogicComponent( ctx )
{
    GM_ = 100.0;
    R_ = 10.0;
    atmHeight_ = 7.0;
    density_ = 1.0;
    viscosity_ = 1.0;

    // Temperature parameters.
    groundT_       =  20.0;
    highAltitudeT_ = -100.0;
}

PlanetForces::~PlanetForces()
{

}

void PlanetForces::Start()
{
    Node * n = GetNode();
    const Vector<SharedPtr<Component> > & comps = n->GetComponents();
    const size_t qty = comps.Size();
    for ( size_t i=0; i<qty; i++ )
    {
        Component * c = comps[i];
        PlanetBase * p = c->Cast<PlanetBase>();
        if ( p )
        {
            planet = SharedPtr<ItemBase>( p );
            break;
        }
    }
}

void PlanetForces::Update( float dt )
{
}

void PlanetForces::applyClose( Block * b )
{
    applyGravity( b );
    applyFriction( b );
}

void PlanetForces::applyFar( Block * b )
{
    clearGravity( b );
    applyFriction( b );
}

void PlanetForces::applyGravity( Block * b )
{
    Vector3d    rel_r;
    Quaterniond rel_q;
    b->relativePose( planet, rel_r, rel_q );
    const Float d = rel_r.Length();
    const Vector3d Fblock = GM_ * rel_r / ( d * d * d );
    // Just write forces.
    // Force is in block ref. frame.
    b->gravity.at = Vector3d::ZERO;
    b->gravity.F  = Fblock;
}

void PlanetForces::clearGravity( Block * b )
{
    b->gravity.at = Vector3d::ZERO;
    b->gravity.F  = Vector3d::ZERO;
}

void PlanetForces::applyFriction( Block * b )
{
    // Small number for considering velocity to be 0.
    static const Float EPS = 1.0e-6;

    const AirMesh & a = b->airMesh;
    const Vector3d    Vmover = b->relV();
    const Quaterniond Q      = b->relQ();
    // Position relative to planet center to compute atmosphere params.
    Vector3d    rel_r;
    Quaterniond rel_q;
    planet->relativePose( b, rel_r, rel_q );
    Float density, viscosity, temperature;
    b->friction.Clear();
    const unsigned qty = a.triangles.Size();
    const bool inAtmosphere = atmosphereParams( rel_r, density, viscosity, temperature );
    if ( !inAtmosphere )
        return;
    // Velocity in block ref. frame.
    const Vector3d V = -Q.Inverse()*Vmover;
    const Float    absV = V.Length();

    // Compute forces in block ref. frame.
    b->friction.Reserve( qty );
    for ( unsigned i=0; i<qty; i++ )
    {
        const Triangle & t = a.triangles[i];
        const Float V_n = -V.DotProduct( t.n );
        // Apply only if (V,n) is negative.
        if ( V_n > EPS )
        {
            // Dynamic pressure force.
            const Vector3d Fn = -(V_n * V_n * density * t.a / absV) * t.n;
            const Vector3d m = (t.v[0] + t.v[1] + t.v[2]) * 0.33333;
            ForceApplied fa;
            fa.at = m;
            fa.F  = Fn;
            b->friction.Push( fa );

            // Viscosity force.
            const Vector3d Fp = (V - V_n*t.n)*t.a*viscosity;
            fa.at = m;
            fa.F  = Fp;
            b->friction.Push( fa );
        }
    }
}

bool PlanetForces::atmosphereParams( const Vector3d & at, Float & density, Float & viscosity, Float & temperature )
{
    const Float h = at.Length() - R_;
    const Float x = h / atmHeight_;
    if ( x > 1.0 )
    {
        density_ = 0.0;
        viscosity = 0.0;
        temperature = 0.0;
        return false;
    }

    if ( h <= 0.0 )
    {
        density     =  density_;
        viscosity   = viscosity_;
        temperature = groundT_;

        return true;
    }

    const Float x2 = x*x;
    const Float _1_x2 = 1.0 - x2;
    density     = _1_x2*density_;
    viscosity   = _1_x2*viscosity_;
    temperature = groundT_*_1_x2 + highAltitudeT_*x2;
    return true;
}




}


