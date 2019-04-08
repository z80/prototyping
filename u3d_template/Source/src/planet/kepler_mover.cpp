
#include "kepler_mover.h"
#include "Eigen/Core"
#include "Eigen/Dense"
//#include <cmath>
//#include <string>

using namespace Eigen;

namespace Osp
{


static void rv2elems( const Float GM, const Vector3d & r, const Vector3d & v,
                      Float & a,        // Semimajor axis
                      Float & e,        // Eccentricity
                      Float & E,        // Eccentric anomaly
                      Float & I,        // Inclination
                      Float & omega,    // Argument of periapsis
                      Float & Omega,    // Longtitude of accending node
                      Float & P,        // Period
                      Float & tau,      // Periapsis crossing time
                      Vector3d & A,
                      Vector3d & B );
static Float nextE( const Float e, const Float M, Float & E, Float & alpha );
static Float solveE( const Float e, const Float M, const Float E );


const Float KeplerMover::TIME_T = 60.0;
const Float KeplerMover::eps    = 1.0e-6;
const int   KeplerMover::iters  = 64;

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

    const Float dt_ = (float)dt;
    timeLow += dt_;
    if ( timeLow > TIME_T )
    {
        timeLow -= TIME_T;
        timeHigh += TIME_T;
    }
    tau = timeLow + timeHigh;
    if ( tau >= P )
    {
        tau -= P;
        timeHigh -= P;
    }
    // Solve for eccentric anomaly "E".
    const Float M = std::sqrt( GM/(a*a*a) )*tau;
    const Float En = solveE( e, M, E );
    E = En;
    // Convert "E" to "f", "r" and "V".
    const Float coE = std::cos(E);
    const Float siE = std::sin(E);
    const Float Rx = a*(coE - e);
    const Float Ry = b*siE;



    // n->SetPosition();
}

bool KeplerMover::launch( const Vector3 & v )
{
    const Vector3 r = relR();
    const Vector3d ev( v.x_, v.y_, v.z_ );
    const Vector3d er( r.x_, r.y_, r.z_ );
    Vector3d A, B;
    rv2elems( GM, er, ev,
              a, e, E, I, w, W, P, tau, A, B );
    timeLow  = 0.0;
    timeHigh = tau;
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





static void rv2elems( const Float GM, const Vector3d & r, const Vector3d & v,
                      Float & a,
                      Float & e,
                      Float & E,        // Eccentric anomaly
                      Float & I,        // Inclination
                      Float & omega,
                      Float & Omega,
                      Float & P,
                      Float & tau,
                      Vector3d & A, 
                      Vector3d & B )
{
    const Float v_2 = v.transpose() * v;
    const Float r_ = std::sqrt( r.transpose() * r );
    const Float Ws = 0.5*v_2 - GM/r_;
    a = -0.5*GM/Ws; // Semimajor axis.

    // Angular momentum.
    const Vector3d L = r.cross( v );
    const Float L_2 = L.transpose() * L;
    // Semi-latus rectum.
    const Float p = L_2 / GM;
    // Eccentricity.
    e = std::sqrt( 1.0 - p/a );

    // Eccentric anomaly
    const Float cosE = (1.0 - r_/a);
    Float sinE = (r.transpose() * v);
    sinE = sinE / ( e * std::sqrt( GM * a ) );
    E = std::atan2( sinE, cosE );

    // Inclination
    const Float sinI = sqrt( L(0)*L(0) + L(2)*L(2) ) / std::sqrt( L_2 );
    const Float cosI = L(3) / std::sqrt( L_2 );
    I = std::atan2( sinI, cosI );

    // Argument of pericenter
    Float t1 = v(0)*L(1) - v(1)*L(0);
    Float t2 = r(2);
    const Float sinw = ( t1 / GM -
                         t2 / r_ ) / (e*sinI);

    t1 = v(2);
    t2 = L(0)*r(1) - L(1)*r(0);
    const Float cosw = ( ( std::sqrt(L_2)*t1 ) / GM - t2 / std::sqrt( L_2  * r_ ) ) / ( e*sinI );
    omega = std::atan2( sinw, cosw );

    // Longtitude of accending node
    t1 = std::sqrt( L_2 ) *sinI;
    const Float cosO =-L(1) / t1;
    const Float sinO = L(0) / t1;
    Omega = std::atan2( sinO, cosO );

    // Orbital period
    t1 = std::sqrt( a*a*a/GM );
    P = 2*3.1415926535*t1;

    // Periapsis crossing time.
    tau = -( E - e*std::sin(E) ) * t1;


    A(0) = a*( std::cos(Omega)*std::cos(omega) - std::sin(Omega)*std::cos(I)*std::sin(omega) );
    A(1) = a*( std::sin(Omega)*std::cos(omega) + std::cos(Omega)*std::cos(I)*std::sin(omega) );
    A(2) = a*std::sin(I)*std::sin(omega);

    B(0) =-a*std::sqrt(1.0 - e*e)*( std::cos(Omega)*std::sin(omega) + 
                                    std::sin(Omega)*std::cos(I)*std::cos(omega) );
    B(1) = a*std::sqrt(1.0 - e*e)*(-std::sin(Omega)*std::sin(omega) + 
                                    std::cos(Omega)*std::cos(I)*std::cos(omega) );
    B(2) = a*std::sqrt(1.0 - e*e)*std::sin(I)*std::cos(omega);
}

static Float nextE( const Float e, const Float M, Float & E, Float & alpha )
{
    Float siE = std::sin(E);
    Float coE = std::cos(E);
    Float ErrN = std::abs( E - e*siE - M );
    Float En_1 = E - (E - e*siE - M)/(1.0 - e*coE)*alpha;
    siE = std::sin(En_1);
    Float ErrN_1 = std::abs( En_1 - e*siE - M );
    int n = 0;
    while ( ErrN_1 >= ErrN )
    {
        ErrN = ErrN_1;

        coE = std::cos(En_1);
        En_1 = En_1 - (En_1 - e*siE - M)/(1.0 - e*coE)*alpha;
        siE = std::sin(En_1);
        ErrN_1 = std::abs( En_1 - e*siE - M );

        n += 1;
        if ( n > KeplerMover::iters )
            break;
    }
    E = En_1;
    return ErrN_1;
}

static Float solveE( const Float e, const Float M, const Float E )
{
    Float En = E;
    Float alpha = 1.0;
    Float err = nextE( e, M, En, alpha );
    int n = 0;
    while ( err >= KeplerMover::eps )
    {
        err = nextE( e, M, En, alpha );

        n += 1;
        if ( n > KeplerMover::iters )
            break;
    }
    return En;
}



}

