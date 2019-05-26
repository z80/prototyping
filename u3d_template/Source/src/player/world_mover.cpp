
#include "world_mover.h"
#include "physics_world_2.h"
#include "MyEvents.h"

namespace Osp
{

WorldMover::WorldMover( Context * ctx )
    : KeplerMover( ctx )
{
    active       = false;
}

WorldMover::~WorldMover()
{

}

void WorldMover::Start()
{
    active = false;

    subscribeToEvents();
}

void WorldMover::Update( float dt )
{
    KeplerMover::Update( dt );

    {
        Scene * s = GetScene();
        GameData * gameData = s->GetComponent<GameData>();

        Node * n = GetNode();
        PhysicsWorld2 * physicsWorld = n->GetComponent<PhysicsWorld2>();
        if ( physicsWorld && gameData )
        {
            const Float dt = static_cast<Float>( gameData->dt ) * GameData::_ONE_SECOND;
            physicsWorld->Update( dt );
        }
    }


    if ( !assembly )
        return;

    // Check the distance to the assembly.
    tryMoveTo();
}

void WorldMover::setR( const Vector3d & new_r )
{
    URHO3D_LOGINFOF( "WorldMover::SetR( %f, %f, %f )", new_r.x_, new_r.y_, new_r.z_ );
    KeplerMover::setR( new_r );
}

void WorldMover::DrawDebugGeometry( DebugRenderer * debug, bool depthTest )
{
    Node * n = GetNode();
    // Draw a cube showing world center position.
    const Matrix3x4 m = n->GetWorldTransform();
    static const float d = 1.0f;
    const Vector3 a1( -d, -d,  d );
    const Vector3 b1(  d, -d,  d );
    const Vector3 c1(  d, -d, -d );
    const Vector3 d1( -d, -d, -d );
    const Vector3 a2( -d,  d,  d );
    const Vector3 b2(  d,  d,  d );
    const Vector3 c2(  d,  d, -d );
    const Vector3 d2( -d,  d, -d );

    debug->AddLine( m*a1, m*b1, Color::RED, false );
    debug->AddLine( m*b1, m*c1, Color::RED, false );
    debug->AddLine( m*c1, m*d1, Color::RED, false );
    debug->AddLine( m*d1, m*a1, Color::RED, false );

    debug->AddLine( m*a2, m*b2, Color::RED, false );
    debug->AddLine( m*b2, m*c2, Color::RED, false );
    debug->AddLine( m*c2, m*d2, Color::RED, false );
    debug->AddLine( m*d2, m*a2, Color::RED, false );

    debug->AddLine( m*a1, m*a2, Color::RED, false );
    debug->AddLine( m*b1, m*b2, Color::RED, false );
    debug->AddLine( m*c1, m*c2, Color::RED, false );
    debug->AddLine( m*d1, m*d2, Color::RED, false );
}

void WorldMover::subscribeToEvents()
{
    //MyEvents::AssemblySelected
    SubscribeToEvent( MyEvents::E_ASSEMBLY_SELECTED, URHO3D_HANDLER( WorldMover, OnAssemblySelected ) );
}

void WorldMover::OnAssemblySelected( StringHash eventType, VariantMap & eventData )
{
    // First get an assembly;
    using namespace MyEvents::AssemblySelected;
    Variant v = eventData[P_ASSEMBLY];
    void * p = v.GetVoidPtr();
    Assembly * a = reinterpret_cast<Assembly *>( p );
    // If assembly is the same just do nothing and return.
    if ( a == assembly.Get() )
        return;

    // Assign the assembly.
    assembly = SharedPtr<Assembly>( a );

    // Physically switch to the new assembly.
    switchTo( assembly );
    // Fill in numbers for world moved and notify all the assemblies.
    switchToEvent( assembly );

    planet = assembly->planet;
}

void WorldMover::switchToEvent( Assembly * assembly )
{
    // Current state.
    const Vector3d curR          = relR();
    const Vector3d curV          = relV();
    const bool     curAtm        = !active;
    const PlanetBase * curPlanet = planet;

    // New state.
    // Here again position is valid only if
    // assembly parent is this exact planet but not an
    // object on the surface.
    const bool     newAtm        = assembly->inAtmosphere;
    Vector3d newR;
    Quaterniond q;
    if ( newAtm )
        assembly->relativePose( assembly->planet->rotator, newR, q );
    else
        assembly->relativePose( assembly->planet->mover, newR, q );
    const Vector3d newV          = assembly->relV();
    const PlanetBase * newPlanet = assembly->planet;

    VariantMap & d = GetEventDataMap();
    using namespace MyEvents::WorldSwitched;

    d[ P_POS_OLD ]    = (void *)&curR;
    d[ P_VEL_OLD ]    = (void *)&curV;
    d[ P_ATM_OLD ]    = (void *)&curAtm;
    d[ P_PLANET_OLD ] = (void *)curPlanet;

    d[ P_POS_NEW ]    = (void *)&newR;
    d[ P_VEL_NEW ]    = (void *)&newV;
    d[ P_ATM_NEW ]    = (void *)&newAtm;
    d[ P_PLANET_NEW ] = (void *)newPlanet;

    SendEvent( MyEvents::E_WORLD_SWITCHED, d );
}

void WorldMover::switchTo( Assembly * assembly )
{
    // Set position to be the same.
    //const Vector3d r = assembly->relR();
    // Here need relative position with respect to planet
    // as assemblies are creted as children of a launch site.

    const bool inAtmosphere = assembly->inAtmosphere;
    // Orbital movement is active
    active = !inAtmosphere;

    Node * selfNode = GetNode();
    PlanetBase * planet = assembly->planet;
    if ( inAtmosphere )
    {
        Vector3d r;
        Quaterniond q;
        assembly->relativePose( planet->rotator, r, q );
        Node * planetNode = planet->rotator->GetNode();

        //Vector3d r2;
        //Quaterniond q2;
        //assembly->relativePose( assembly->planet->rotator, r2, q2 );

        selfNode->SetParent( planetNode );

        //assembly->relativePose( assembly->planet->rotator, r2, q2 );
        //assembly->relativePose( this, r2, q2 );

        setR( r );

        //assembly->relativePose( assembly->planet->rotator, r2, q2 );
        // This is not changed after setting
        // parent and assigning "r".
        //assembly->relativePose( this, r2, q2 );
        //q2 = Quaterniond::IDENTITY;
    }
    else
    {
        Vector3d r;
        Quaterniond q;
        assembly->relativePose( planet->mover, r, q );
        Node * planetNode = planet->mover->GetNode();
        selfNode->SetParent( planetNode );

        setR( r );
        // If assembly is in orbit set orbital movement.
        const Vector3d v = assembly->relV();
        launch( v );
    }
}

void WorldMover::tryMoveTo()
{
    if ( !assembly )
        return;

    Vector3d dr;
    Quaterniond dq;
    assembly->relativePose( this, dr, dq );
    const Float d = dr.Length();
    const bool inAtmosphere = !active;
    if ( (d < GameData::DIST_WHEN_NEED_MOVE) && ( inAtmosphere == assembly->inAtmosphere) )
        return;

    Vector3d dv;
    const Vector3d v0 = relV();
    const Vector3d r1 = relR() + dr;
    setR( r1 );
    if ( !inAtmosphere )
    {
        const Vector3d v1 = assembly->relV();
        active == true;
        launch( v1 );

        dv = v0 - v1;
    }
    else
        dv = Vector3d::ZERO;

    moveToEvent( r1, -dr, dv );
}

void WorldMover::moveToEvent( const Vector3d & r, const Vector3d & dr, const Vector3d & dv )
{
    VariantMap & d = GetEventDataMap();
    using namespace MyEvents::WorldMoved;
    d[P_POS_NEW] = (void *)&r;
    d[P_POS_ADJ] = (void *)&dr;
    d[P_VEL_ADJ] = (void *)&dv;
    SendEvent( MyEvents::E_WORLD_MOVED, d );
}



}
