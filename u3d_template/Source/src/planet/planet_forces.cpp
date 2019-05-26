
#include "planet_forces.h"
#include "block.h"
#include "planet_base.h"

namespace Osp
{

PlanetForces::PlanetForces( Context * ctx )
    : LogicComponent( ctx )
{
    GM_ = 1.0;
    R_  = 10.0;
    atmHeight_  = 7.0;
    density_    = 1.0;
    viscosity_  = 1.0;
    densityB_   = 0.5;
    viscosityB_ = 0.5;

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
    planet->relativePose( b, rel_r, rel_q, true );
    const Float d = rel_r.Length();
    const Vector3d Fblock = GM_ * rel_r / ( d * d * d );
    // Just write forces.
    // Force is in block ref. frame.
    b->gravity.at = Vector3d::ZERO;
    b->gravity.F  = Fblock;


    // Debugging relative transformation comutation.
//    {
//        Node * blockN  = b->GetNode();
//        Node * planetN = planet->GetNode();
//        Matrix3x4 blockT  = blockN->GetWorldTransform();
//        Matrix3x4 planetT = planetN->GetWorldTransform();
//        // PlanetIn block coordinates.
//        Matrix3x4 relPlanetT = blockT.Inverse() * planetT;
//        Vector3 r, s;
//        Quaternion q;
//        relPlanetT.Decompose( r, q, s );
//        URHO3D_LOGINFOF( "Relative pose" );
//        URHO3D_LOGINFOF( "     r: (%f, %f, %f)", rel_r.x_, rel_r.y_, rel_r.z_ );
//        URHO3D_LOGINFOF( "     q: (%f, %f, %f, %f)", rel_q.w_, rel_q.x_, rel_q.y_, rel_q.z_ );
//        URHO3D_LOGINFOF( "Should be" );
//        URHO3D_LOGINFOF( "     r: (%f, %f, %f)", r.x_, r.y_, r.z_ );
//        URHO3D_LOGINFOF( "     q: (%f, %f, %f, %f)", q.w_, q.x_, q.y_, q.z_ );
//    }
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
    Float densityF, viscosityF,
          densityB, viscosityB,
          temperature;
    b->friction.Clear();
    const unsigned qty = a.triangles.Size();
    const bool inAtmosphere = atmosphereParams( rel_r, temperature, densityF, viscosityF,
                                                                    densityB, viscosityB );
    if ( !inAtmosphere )
        return;
    // Velocity in block ref. frame.
    const Vector3d V = Q.Inverse()*Vmover;
    const Float    absV = V.Length();

    // Compute forces in block ref. frame.
    b->friction.Reserve( qty );
    for ( unsigned i=0; i<qty; i++ )
    {
        const Triangle & t = a.triangles[i];
        const Float V_n = V.DotProduct( t.n );
        ForceApplied fa;

        // Dynamic pressure force.
        const Float density = ( V_n >= 0.0 ) ? densityF : densityB;
        const Vector3d F_dynamic_pressure = -(V_n * V_n * density * t.a) * t.n;


        // Viscosity force.
        const Float viscosity = ( V_n >= 0.0 ) ? viscosityF : viscosityB;
        const Vector3d F_viscosity = (V_n*t.n - V)*t.a*viscosity;

        fa.F  = F_dynamic_pressure + F_viscosity;

        const Vector3d m = (t.v[0] + t.v[1] + t.v[2]) * 0.33333;
        fa.at = m;
        b->friction.Push( fa );
    }
}

bool PlanetForces::atmosphereParams( const Vector3d &at, Float & temperature, Float & densityF, Float & viscosityF,
                                                                              Float & densityB, Float & viscosityB )
{
    const Float h = at.Length() - R_;
    const Float x = h / atmHeight_;
    if ( x > 1.0 )
    {
        densityF    = 0.0;
        viscosityF  = 0.0;
        densityB    = 0.0;
        viscosityB  = 0.0;
        temperature = 0.0;
        return false;
    }

    if ( h <= 0.0 )
    {
        densityF    = density_;
        viscosityF  = viscosity_;
        densityB    = densityB_;
        viscosityB  = viscosityB_;
        temperature = groundT_;

        return true;
    }

    const Float x2 = x*x;
    const Float _1_x2 = 1.0 - x2;
    densityF     = _1_x2*density_;
    viscosityF   = _1_x2*viscosity_;
    densityB     = _1_x2*densityB_;
    viscosityB   = _1_x2*viscosityB_;
    temperature = groundT_*_1_x2 + highAltitudeT_*x2;
    return true;
}




}


