
#include "kepler_rotator.h"
#include "Eigen/Core"
#include "Eigen/Geometry"

namespace Osp
{

KeplerRotator::KeplerRotator( Context * ctx )
    : ItemBase( ctx )
{
    yaw   = 0.0;
    pitch = 0.0;
    roll  = 0.0;
    period = GameData::ONE_SECOND * 60;
}

KeplerRotator::~KeplerRotator()
{
}

void KeplerRotator::launch( Timestamp periodSec, Float yaw, Float pitch, Float roll )
{
    period = GameData::ONE_SECOND * periodSec;
    this->yaw = yaw;
    this->pitch = pitch;
    this->roll  = roll;
    computeBaseRotation();
}

void KeplerRotator::Start()
{
    gameData = GetSubsystem<GameData>();
    if ( !gameData )
        URHO3D_LOGERROR( "Failed to get GameData instance" );


}

void KeplerRotator::Update( float dt )
{
    if ( !gameData )
        return;
    const Timestamp t = gameData->time;

    // Compute current period remainder.
    Float r;
    if ( period > 0 )
    {
        const Timestamp tauI = t % period;
        const Float a = static_cast<Float>( tauI ) / static_cast<Float>( period ) * PI2;
        r = roll + a;
    }
    else
        r = roll;


    const Float co2Roll = std::cos( r * 0.5 );
    const Float si2Roll = std::sin( r * 0.5 );
    const Quaterniond qRoll( co2Roll, 0.0, si2Roll, 0.0 );
    const Quaterniond qe = qBase * qRoll;

    setQ( qe );
}

void KeplerRotator::computeBaseRotation()
{
    // Base quaternion.
    {
        const Float co2Yaw = std::cos( yaw * 0.5 );
        const Float si2Yaw = std::sin( yaw * 0.5 );
        const Eigen::Quaterniond qYaw( co2Yaw, si2Yaw, 0.0, 0.0 );

        const Float co2Pitch = std::cos( pitch * 0.5 );
        const Float si2Pitch = std::sin( pitch * 0.5 );
        const Eigen::Quaterniond qPitch( co2Pitch, 0.0, 0.0, si2Pitch );

        const Eigen::Quaterniond Q = qYaw * qPitch;
        qBase = Quaterniond( Q.w(), Q.x(), Q.y(), Q.z() );
    }


    // Set angular velocity.
    // It is assumed to be constant for this simplified model.
    if ( period > 0 )
    {
        const Float w = PI2 / static_cast<Float>( period );
        Vector3d vw( 0.0, w, 0.0 );

        vw = qBase * vw;

        setW( vw );
    }
    else
        setW( Vector3d::ZERO );
}

}



