
#include "kepler_rotator.h"
#include "Eigen/Core"
#include "Eigen/Geometry"

namespace Osp
{

KeplerRotator::KeplerRotator( Context * ctx )
    : LogicComponent( ctx )
{
    yaw   = 0.0;
    pitch = 0.0;
    roll  = 0.0;
    period = GameData::ONE_SECOND * 60;
}

KeplerRotator::~KeplerRotator()
{
}

void KeplerRotator::Start()
{
    gameData = GetSubsystem<GameData>();
    if ( !gameData )
        URHO3D_LOGERROR( "Failed to get GameData instance" );

    Node * n = GetNode();
    static Vector< SharedPtr<Component> > comps;
    comps = n->GetComponents();
    const size_t qty = comps.Size();
    PlanetBase * p = nullptr;
    for ( size_t i=0; i<qty; i++ )
    {
        Component * c = comps[i];
        PlanetBase * p = c->Cast<PlanetBase>();
        if ( !p )
            continue;
    };

    if ( !p )
        URHO3D_LOGERROR( "Failed to retrieve PlanetBase instance" );

    if ( p )
        planet = SharedPtr<PlanetBase>( p );
}

void KeplerRotator::Update( float dt )
{
    if ( !gameData )
        return;
    const Timestamp t = gameData->time;
    const Float co2Yaw = std::cos( yaw * 0.5 );
    const Float si2Yaw = std::sin( yaw * 0.5 );
    const Eigen::Quaterniond qYaw( co2Yaw, si2Yaw, 0.0, 0.0 );

    const Float co2Pitch = std::cos( pitch * 0.5 );
    const Float si2Pitch = std::sin( pitch * 0.5 );
    const Eigen::Quaterniond qPitch( co2Pitch, 0.0, 0.0, si2Pitch );

    // Compute current period remainder.
    const Timestamp tauI = t % period;
    const Float a = static_cast<Float>( tauI ) / static_cast<Float>( period ) * PI2;
    const Float r = roll + a;

    const Float co2Roll = std::cos( r * 0.5 );
    const Float si2Roll = std::sin( r * 0.5 );
    const Eigen::Quaterniond qRoll( co2Roll, 0.0, si2Roll, 0.0 );

    const Eigen::Quaterniond qe = qYaw * qPitch * qRoll;
    if ( !planet )
        return;
    const Quaternion q( qe.w(), qe.x(), qe.y(), qe.z() );
    planet->setQ( q );
}

}



