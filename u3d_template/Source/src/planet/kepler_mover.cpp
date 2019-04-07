
#include "kepler_mover.h"
//#include "Eigen/Core"
//#include "Eigen/Dense"
//#include <cmath>
//#include <string>

//using namespace Eigen;

namespace Osp
{

/*
static void rv2elems( const Vector3d & r, const Vector3d & v, 
                      Float & a,
                      Float & e,
                      Float & E,
                      Float & I,
                      Float & omega,
                      Float & Omega,
                      Float & P,
                      Float & tau,
                      Vector3d & A, 
                      Vector3d & B );
*/

KeplerMover::KeplerMover( Context * ctx )
    : ItemBase( ctx )
{
    active = false;
    GM = 1.0;
    active = false;
}

KeplerMover::~KeplerMover()
{
}

void KeplerMover::Update( float dt )
{
    if ( !active )
      return;

    // Compute current position and velocity.
    // Apply position to it's node.
    Node * n = GetNode();
    if ( !n )
        return;
    // n->SetPosition();
}

bool KeplerMover::launch( const Vector3 & v )
{
}

void KeplerMover::stop()
{
}

Vector3 KeplerMover::relR() const
{
}

Vector3 KeplerMover::relV() const
{
}




/*
static void rv2elems( const Vector3d & r, const Vector3d & v, 
                      Float & a,
                      Float & e,
                      Float & E,
                      Float & I,
                      Float & omega,
                      Float & Omega,
                      Float & P,
                      Float & tau,
                      Vector3d & A, 
                      Vector3d & B )
{
    const Float v_2 = v.transposed() * v;
    const Float r_ = sqrt( r.transposed() * r );
    const Float Ws = 0.5*v_2 - GM/r_;
    a = -0.5*GM/Ws; // Semimajor axis.

    // Angular momentum.
    const Vector3d L = r.cross( v );
    const Float L_2 = L.transposed() * L;
    // Semi-latus rectum.
    const Float p = L_2 / GM;
    // Eccentricity.
    e = std::sqrt( 1.0 - p/a );

    // Eccentric anomaly
    const Float cosE = (1.0 - r_/a);
    const Float sinE = (r.transposed() * v) / ( e * std::sqrt( GM * a ) );
    E = std::atan2( sinE, cosE );

    // Inclination
    const Float sinI = sqrt( L(0)*L(0) + L(2)*L(2) ) / std::sqrt( L_2 );
    const Float cosI = L(3) / std::sqrt( L_2 );
    I = std::atan2( sinI, cosI );

    // Argument of pericenter
    const Float sinw = ( ( v(0)*L(1) - v(1)*L(0) ) / GM - 
                       r(2)/r ) / (e*sinI);

    const Float cosw = ( ( std::sqrt(L_2)*v(2) ) / GM - ( L(0)*r(1) - L(1)*r(0) ) / std::sqrt( L_2  * r ) ) / ( e*sinI );
    omega = std::atan2( sinw, cosw );

    // Longtitude of accending node
    const Float cosO = -L(1) / ( std::sqrt( L_2 ) *sinI );
    const Float sinO = L(0) / ( std::sqrt( L_2 ) * sinI );
    Omega = std::atan2( sinO, cosO );

    // Orbital period
    P = 2*.31415926535*std::sqrt( a*a*a/GM );

    // Periapsis crossing time.
    tau = -( E - e*std::sin(E) )/std::sqrt( GM/(a*a*a) );


    A(0) = a*( std::cos(Omega)*std::cos(omega) - std::sin(Omega)*std::cos(I)*std::sin(omega) );
    A(1) = a*( std::sin(Omega)*std::cos(omega) + std::cos(Omega)*std::cos(I)*std::sin(omega) );
    A(2) = a*std::sin(I)*std::sin(omega);

    B(0) =-a*std::sqrt(1.0 - e*e)*( std::cos(Omega)*std::sin(omega) + 
                                    std::sin(Omega)*std::cos(I)*std::cos(omega) );
    B(1) = a*std::sqrt(1.0 - e*e)*(-std::sin(Omega)*std::sin(omega) + 
                                    std::cos(Omega)*std::cos(I)*std::cos(omega) );
    B(2) = a*std::sqrt(1.0 - e*e)*std::sin(I)*std::cos(omega);
}
*/


}

