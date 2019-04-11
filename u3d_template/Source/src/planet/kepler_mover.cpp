
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
static Float speed( Float GM, Float a, Float r );

// This is a special one assuming (e < 1.0).
static void ellipticInit( KeplerMover * km, Float GM, Float a, Float e, Float Omega, Float I, Float omega, Float E );

// A generic one which computes "e".
static void genericInit( KeplerMover * km, const Vector3d & r, const Vector3d & v );

static void ellipticInit( KeplerMover * km, const Vector3d & r, const Vector3d & v );
static void ellipticProcess( KeplerMover * km, Float t, Vector3d & r, Vector3d & v );
static Float ellipticNextE( const Float e, const Float M, Float & E, Float & alpha );
static Float ellipticSolveE( const Float e, const Float M, const Float E );

//static void hyperbolicInit( KeplerMover * km, Float GM, Float a, Float e, Float Omega, Float I, Float omega, Float E );
static void hyperbolicInit( KeplerMover * km, const Vector3d & r, const Vector3d & v );
static void hyperbolicProcess( KeplerMover * km, Float t, Vector3d & r, Vector3d & v );
static Float hyperbolicNextE( const Float e, const Float M, Float & E, Float & alpha );
static Float hyperbolicSolveE( const Float e, const Float M, const Float E );

static void parabolicInit(KeplerMover * km, const Vector3d & r, const Vector3d & v );
static void parabolicProcess( KeplerMover * km, Float t, Vector3d & r, Vector3d & v );


const Float KeplerMover::TIME_T = 60.0;
const Float KeplerMover::eps    = 1.0e-6;
const int   KeplerMover::iters  = 64;
static const Float _2PI = 2.0 * 3.1415926535;

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


    Vector3d r, v;
    if ( e < (1.0 - eps) )
        // Elliptic
        ellipticProcess( this, tau, r, v );
    else if ( e > (1.0 + eps) )
        // Hyperbolic
        ;
    else
        // Parabolic
        ;
    const Vector3 rf( r(0), r(1), r(2) );
    n->SetPosition( rf );
}

void KeplerMover::initKepler( Float GM, Float a, Float e, Float Omega, Float I, Float omega, Float E )
{
    if ( e < (1.0 - eps) )
        // Elliptic
        ellipticInit( this, GM, a, e, Omega, I, omega, E );
    else if ( e > (1.0 + eps) )
        // Hyperbolic
        ;
    else
        // Parabolic
        ;
    active = true;
}

bool KeplerMover::launch( const Vector3 & v )
{
    const Vector3 r = relR();
    const Vector3d ev( v.x_, v.y_, v.z_ );
    const Vector3d er( r.x_, r.y_, r.z_ );
    Vector3d A, B;
    rv2elems( GM, er, ev,
              a, e, E, I, omega, Omega, P, tau, A, B );
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
    P = _2PI*t1;

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

static Float ellipticNextE( const Float e, const Float M, Float & E, Float & alpha )
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
        alpha *= 0.5;
    }
    E = En_1;
    return ErrN_1;
}

static Float ellipticSolveE( const Float e, const Float M, const Float E )
{
    Float En = E;
    Float alpha = 1.0;
    Float err = ellipticNextE( e, M, En, alpha );
    int n = 0;
    while ( err >= KeplerMover::eps )
    {
        err = ellipticNextE( e, M, En, alpha );

        n += 1;
        if ( n > KeplerMover::iters )
            break;
    }
    if ( En > _2PI )
        En -= _2PI;
    else if ( En < 0.0 )
        En += _2PI;
    return En;
}

static Float speed( Float GM, Float a, Float r )
{
    const Float v = std::sqrt( GM*( 2.0/r - 1.0/a ) );
    return v;
}

static void ellipticInit( KeplerMover * km, Float GM, Float a, Float e, Float Omega, Float I, Float omega, Float E )
{
    km->GM = GM;
    km->a  = a;
    km->e  = e;
    km->Omega  = Omega;
    km->I  = I;
    km->omega  = omega;
    km->E  = E;
    const Float n = std::sqrt( GM/(a*a*a) );
    km->P = _2PI/n;
    // Current orbital time after periapsis.
    const Float M = E - e*std::sin(E);
    const Float t = M / n;

    km->timeHigh = t;
    km->timeLow  = 0.0;
}

static void genericInit( KeplerMover * km, const Vector3d & _r, const Vector3d & _v )
{
    const Float GM = km->GM;

    // First adjust vectors by 90 degrees to make it as if Oz was vertical.
    const Float _2 = 1.0/std::sqrt(2.0);
    const Quaterniond q( _2, _2, 0.0, 0.0 );
    const Vector3d r = q * _r;
    const Vector3d v = q * _v;

    const Float v_2 = v.transpose() * v;
    const Float r_ = std::sqrt( r.transpose() * r );
    const Float Ws = 0.5*v_2 - GM/r_;
    const Float a = -0.5*GM/Ws; // Semimajor axis.
    km->a = a;

    // Angular momentum.
    const Vector3d L = r.cross( v );
    const Float L_2 = L.transpose() * L;
    // Semi-latus rectum.
    const Float p = L_2 / GM;
    // Eccentricity.
    const Float e = std::sqrt( 1.0 - p/a );
    km->e = e;

    if ( e > (1.0 + KeplerMover::eps) )
        hyperbolicInit( km, r, v );
    else if ( e < (1.0 -KeplerMover::eps) )
        ellipticInit( km, r, v );
    else
        parabolicInit( km, r, v );
}

static void ellipticInit( KeplerMover * km, const Vector3d & _r, const Vector3d & _v )
{
    const Float GM = km->GM;

    // First adjust vectors by 90 degrees to make it as if Oz was vertical.
    const Float _2 = 1.0/std::sqrt(2.0);
    const Quaterniond q( _2, _2, 0.0, 0.0 );
    const Vector3d r = q * _r;
    const Vector3d v = q * _v;

    const Float v_2 = v.transpose() * v;
    const Float r_ = std::sqrt( r.transpose() * r );
    const Float Ws = 0.5*v_2 - GM/r_;
    const Float a = -0.5*GM/Ws; // Semimajor axis.
    km->a = a;

    // Angular momentum.
    const Vector3d L = r.cross( v );
    const Float L_2 = L.transpose() * L;
    // Semi-latus rectum.
    const Float p = L_2 / GM;
    // Eccentricity.
    const Float e = std::sqrt( 1.0 - p/a );
    km->e = e;

    // Eccentric anomaly
    const Float cosE = (1.0 - r_/a);
    Float sinE = (r.transpose() * v);
    sinE = sinE / ( e * std::sqrt( GM * a ) );
    const Float E = std::atan2( sinE, cosE );
    km->E = E;

    // Inclination
    const Float sinI = sqrt( L(0)*L(0) + L(2)*L(2) ) / std::sqrt( L_2 );
    const Float cosI = L(3) / std::sqrt( L_2 );
    km->I = std::atan2( sinI, cosI );

    // Argument of pericenter
    Float t1 = v(0)*L(1) - v(1)*L(0);
    Float t2 = r(2);
    const Float sinw = ( t1 / GM -
                         t2 / r_ ) / (e*sinI);

    t1 = v(2);
    t2 = L(0)*r(1) - L(1)*r(0);
    const Float cosw = ( ( std::sqrt(L_2)*t1 ) / GM - t2 / std::sqrt( L_2  * r_ ) ) / ( e*sinI );
    km->omega = std::atan2( sinw, cosw );

    // Longtitude of accending node
    t1 = std::sqrt( L_2 ) *sinI;
    const Float cosO =-L(1) / t1;
    const Float sinO = L(0) / t1;
    km->Omega = std::atan2( sinO, cosO );

    // Orbital period
    t1 = std::sqrt( a*a*a/GM );
    km->P = _2PI*t1;

    // Periapsis crossing time.
    km->tau = -( E - e*std::sin(E) ) * t1;
    km->timeHigh = km->tau;
    km->timeLow  = 0.0;
}

static void ellipticProcess( KeplerMover * km, Float t, Vector3d & r, Vector3d & v )
{
    // Solve for eccentric anomaly "E".
    const Float a = km->a;
    const Float GM = km->GM;
    const Float n = std::sqrt( (a*a*a)/GM );
    const Float M = t/n;
    Float & E = km->E;

    const Float e = km->e;
    E = ellipticSolveE( e, M, E );
    // Convert "E" to "f", "r" and "V".
    const Float coE = std::cos(E);
    const Float siE = std::sin(E);
    const Float Rx = a*(coE - e);
    const Float b = a * std::sqrt( 1 - e*e );
    const Float Ry = b*siE;

    Vector3d ax( 1.0, 0.0, 0.0 );
    Vector3d ay( 0.0, 1.0, 0.0 );
    const Float Omega = km->Omega;
    const Float I     = km->I;
    const Float omega = km->omega;
    const Float _2 = 1.0/std::sqrt(2.0);
    const Quaterniond q( _2, -_2, 0.0, 0.0 );
    const Quaterniond qW( std::cos(Omega/2), 0.0, 0.0, std::sin(Omega/2) );
    const Quaterniond qI( std::cos(I/2), 0.0, 0.0, std::sin(I/2) );
    const Quaterniond qw( std::cos(omega/2), 0.0, 0.0, std::sin(omega/2) );
    const Quaterniond Q = q * qW * qI * qw;
    ax = Q * ax;
    ay = Q * ay;
    // Position at orbit.
    r = (ax * Rx) + (ay * Ry);
}

/*static void hyperbolicInit( KeplerMover * km, Float GM, Float a, Float e, Float Omega, Float I, Float omega, Float E )
{
    km->GM = GM;
    km->a  = a;
    km->e  = e;
    km->Omega = Omega;
    km->I = I;
    km->omega = omega;
    km->E  = E;

    const Float n = std::sqrt( GM/(a*a*a) );
    const Float M = e * std::sinh(E) - E;
    t = M / n;

    km->timeHigh = t;
    km->timeLow  = 0.0;
}*/

static void hyperbolicInit( KeplerMover * km, const Vector3d & _r, const Vector3d & _v )
{
    // https://en.wikipedia.org/wiki/Hyperbolic_trajectory
    const Float GM = km->GM;

    // First adjust vectors by 90 degrees to make it as if Oz was vertical.
    const Float _2 = 1.0/std::sqrt(2.0);
    const Quaterniond q( _2, _2, 0.0, 0.0 );
    const Vector3d r = q * _r;
    const Vector3d v = q * _v;

    const Float v_2 = v.transpose() * v;
    const Float r_ = std::sqrt( r.transpose() * r );
    const Float Ws = 0.5*v_2 - GM/r_;
    const Float a = -0.5*GM/Ws; // Semimajor axis.
    km->a = a;

    // Angular momentum.
    const Vector3d L = r.cross( v );
    const Float L_2 = L.transpose() * L;
    // Semi-latus rectum.
    const Float p = L_2 / GM;
    km->l = p;
    // Eccentricity.
    const Float e = std::sqrt( 1.0 - p/a );
    km->e = e;

    // True anomaly.
    // r = l/(1 + e*cos(f));
    // Solve for "f" and define the sign using dot product of (r, v).
    Float f = std::acos( (p/r_ - 1.0)/e );
    const Float rv = r.transpose() * v;
    const bool positive = (rv >= 0.0);
    f = positive ? f : (-f);
    km->f = f;

    // Eccentric anomaly.
    // cosh(E) = ( cos(f) + e ) / ( 1 + e * cos(f) );
    const Float coF = std::cos(f);
    Float E = std::acosh( (coF + e)/(1.0 + e*coF) );
    E = positive ? E : (-E);
    km->E = E;

    // Mean anomaly and time.
    // M = sqrt( -GM/(a*a*a) )*t = e*sinhE - E;
    const Float n = std::sqrt( -(a*a*a)/GM );
    const Float t = (e*std::sinh(E) - E) * n;
    km->timeHigh = t;
    km->tau      = t;
    km->timeLow  = 0.0;
}

static void hyperbolicProcess( KeplerMover * km, Float t, Vector3d & r, Vector3d & v )
{
    // Solve for eccentric anomaly "E".
    const Float a = km->a;
    const Float GM = km->GM;
    const Float n = std::sqrt( -(a*a*a)/GM );
    const Float M = t/n;
    Float & E = km->E;

    const Float e = km->e;
    E = hyperbolicSolveE( e, M, E );
    // Convert "E" to "f", "r" and "V".
    const Float coE = std::cosh(E);

    // cosh(E) = (cos(f) + e)/(1 + e*cos(f))
    Float f = std::acos( (e - coE)/(e*coE - 1.0) );
    f = (E >= 0.0) ? f : (-f);
    km->f = f;

    // r = l/(1 + e*cos(f))
    const Float siF = std::sin(f);
    const Float coF = std::cos(f);
    const Float r_ = km->l / ( 1.0 + e*coF );
    const Float Rx = r_ * coF;
    const Float Ry = r_ * siF;

    // Computing real 3d coordinates.
    Vector3d ax( 1.0, 0.0, 0.0 );
    Vector3d ay( 0.0, 1.0, 0.0 );
    const Float Omega = km->Omega;
    const Float I     = km->I;
    const Float omega = km->omega;
    const Float _2 = 1.0/std::sqrt(2.0);
    const Quaterniond q( _2, -_2, 0.0, 0.0 );
    const Quaterniond qW( std::cos(Omega/2), 0.0, 0.0, std::sin(Omega/2) );
    const Quaterniond qI( std::cos(I/2), 0.0, 0.0, std::sin(I/2) );
    const Quaterniond qw( std::cos(omega/2), 0.0, 0.0, std::sin(omega/2) );
    const Quaterniond Q = q * qW * qI * qw;
    ax = Q * ax;
    ay = Q * ay;
    // Position at orbit.
    r = (ax * Rx) + (ay * Ry);
}

static Float hyperbolicNextE( const Float e, const Float M, Float & E, Float & alpha )
{
    Float siE = std::sinh(E);
    Float coE = std::cosh(E);
    Float ErrN = std::abs( e*siE - E - M );
    Float En_1 = E - (e*siE - E - M)/(e*coE - 1.0)*alpha;
    siE = std::sin(En_1);
    Float ErrN_1 = std::abs( e*siE - En_1 - M );
    int n = 0;
    while ( ErrN_1 >= ErrN )
    {
        ErrN = ErrN_1;

        coE = std::cos(En_1);
        En_1 = En_1 - (e*siE - En_1 - M)/(e*coE - 1.0)*alpha;
        siE = std::sin(En_1);
        ErrN_1 = std::abs( e*siE - En_1 - M );

        n += 1;
        if ( n > KeplerMover::iters )
            break;
        alpha *= 0.5;
    }
    E = En_1;
    return ErrN_1;
}

static Float hyperbolicSolveE( const Float e, const Float M, const Float E )
{
    Float En = E;
    Float alpha = 1.0;
    Float err = hyperbolicNextE( e, M, En, alpha );
    int n = 0;
    while ( err >= KeplerMover::eps )
    {
        err = hyperbolicNextE( e, M, En, alpha );

        n += 1;
        if ( n > KeplerMover::iters )
            break;
    }
    return En;
}

static void parabolicInit(KeplerMover * km, const Vector3d & _r, const Vector3d & _v )
{
    // https://en.wikipedia.org/wiki/Parabolic_trajectory
    const Float GM = km->GM;

    // First adjust vectors by 90 degrees to make it as if Oz was vertical.
    const Float _2 = 1.0/std::sqrt(2.0);
    const Quaterniond q( _2, _2, 0.0, 0.0 );
    const Vector3d r = q * _r;
    const Vector3d v = q * _v;

    const Float v_2 = v.transpose() * v;
    const Float r_ = std::sqrt( r.transpose() * r );
    const Float Ws = 0.5*v_2 - GM/r_;
    const Float a = -0.5*GM/Ws; // Semimajor axis.
    km->a = a;

    // Angular momentum.
    const Vector3d L = r.cross( v );
    const Float L_2 = L.transpose() * L;
    // Semi-latus rectum.
    const Float p = L_2 / GM;
    km->l = p;
    // Eccentricity.
    const Float e = std::sqrt( 1.0 - p/a );
    km->e = e;

    // True anomaly.
    // r = l/(1 + cos(f));
    // Solve for "f" and define the sign using dot product of (r, v).
    Float f = std::acos( (p/r_ - 1.0) );
    const Float rv = r.transpose() * v;
    const bool positive = (rv >= 0.0);
    f = positive ? f : (-f);
    km->f = f;

    // Mean anomaly and time.
    // t = sqrt( l^3/GM )*(D + D^3/3)
    // D = tan(f/2).
    const Float n = std::sqrt( (p*p*p)/GM );
    const Float D = std::tan(0.5*f);
    const Float t = 0.5*n*( D + D*D*D/3.0 );
    km->timeHigh = t;
    km->tau      = t;
    km->timeLow  = 0.0;
}

static void parabolicProcess( KeplerMover * km, Float t, Vector3d & r, Vector3d & v )
{
    const Float GM = km->GM;
    const Float p = km->l;
    const Float rp = 0.5*p;
    const Float n = std::sqrt( GM/(2.0*rp*rp*rp) );
    const Float A = 3.0/2.0*n*t;
    const Float B = std::pow( A + std::sqrt(A*A + 1.0), 1.0/3.0 );
    const Float f = 2.0 * std::atan( B - 1.0/B );

    const Float coF = std::cos(f);
    const Float siF = std::sin(f);
    const Float r_ = p/(1.0 + std::cos(f));
    const Float Rx = r_ * coF;
    const Float Ry = r_ * siF;

    // Computing real 3d coordinates.
    Vector3d ax( 1.0, 0.0, 0.0 );
    Vector3d ay( 0.0, 1.0, 0.0 );
    const Float Omega = km->Omega;
    const Float I     = km->I;
    const Float omega = km->omega;
    const Float _2 = 1.0/std::sqrt(2.0);
    const Quaterniond q( _2, -_2, 0.0, 0.0 );
    const Quaterniond qW( std::cos(Omega/2), 0.0, 0.0, std::sin(Omega/2) );
    const Quaterniond qI( std::cos(I/2), 0.0, 0.0, std::sin(I/2) );
    const Quaterniond qw( std::cos(omega/2), 0.0, 0.0, std::sin(omega/2) );
    const Quaterniond Q = q * qW * qI * qw;
    ax = Q * ax;
    ay = Q * ay;
    // Position at orbit.
    r = (ax * Rx) + (ay * Ry);
}


}

