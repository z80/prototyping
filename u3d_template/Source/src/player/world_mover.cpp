
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

    // Check if need to switch to orbiting or surfacing.
    // Check if time to leave/enter world.
    // These happen without physical displacements.
    if ( active )
    {
        // Check if need to switch to ground.
        // Or if need t oswitch closest planet.
        const bool need = needGround();
        if ( need )
            switchToGrounding();
        else
            checkInfluence();
    }
    else
    {
        const bool need = needOrbit();
        if ( need )
            switchToOrbiting();
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

bool WorldMover::needOrbit() const
{
    if ( (!planet) || (active) )
        return false;
    const bool canOrbit = planet->canOrbit( this );
    return canOrbit;
}

bool WorldMover::needGround() const
{
    if ( (!planet) || (!active) )
        return false;
    const bool canOrbit = planet->canOrbit( this );
    return canOrbit;
}

void WorldMover::switchToOrbiting()
{
    Vector3d r, v, w;
    Quaterniond q;
    this->relativeAll( planet->mover, r, q, v, w, true );
    Node * planetNode = planet->mover->GetNode();
    Node * selfNode = GetNode();
    selfNode->SetParent( planetNode );

    setR( r );
    setQ( q );
    setW( Vector3d::ZERO );
    launch( v );

    // Send notification event.
    VariantMap & d = GetEventDataMap();
    using namespace MyEvents::WorldStateChanged;

    const bool orbiting = true;

    d[ P_ORBITING ]   = (void *)&orbiting;
    d[ P_PLANET_OLD ] = (void *)planet;
    d[ P_PLANET_NEW ] = (void *)planet;

    SendEvent( MyEvents::E_WORLD_STATE_CHANGED, d );
}

void WorldMover::switchToGrounding()
{
    Vector3d r, v, w;
    Quaterniond q;
    this->relativeAll( planet->rotator, r, q, v, w, true );
    Node * planetNode = planet->rotator->GetNode();
    Node * selfNode = GetNode();
    selfNode->SetParent( planetNode );

    v = Vector3d::ZERO;
    setR( r );
    setQ( q );
    setV( Vector3d::ZERO );
    setW( Vector3d::ZERO );
    // Don't use Kepler mover
    active = false;


    // Send notification event.
    VariantMap & d = GetEventDataMap();
    using namespace MyEvents::WorldStateChanged;

    const bool orbiting = false;

    d[ P_ORBITING ]   = (void *)&orbiting;
    d[ P_PLANET_OLD ] = (void *)planet;
    d[ P_PLANET_NEW ] = (void *)planet;

    SendEvent( MyEvents::E_WORLD_STATE_CHANGED, d );
}

void WorldMover::checkInfluence()
{
    if ( !active )
        return;

    PlanetBase * closestP = planetOfInfluence();
    if ( planet == closestP )
        return;

    // Need to switch parent planet.
    // Compute relative pose and velocity.
    Vector3d rel_r, rel_v, rel_w;
    Quaterniond rel_q;
    const bool ok = relativeAll( closestP, rel_r, rel_q, rel_v, rel_w, true );
    if ( !ok )
        URHO3D_LOGERROR( "Failed to compute relativeAll()" );

    Node * selfNode = GetNode();
    Node * planetNode = closestP->mover->GetNode();
    selfNode->SetParent( planetNode );
    launch( rel_v );


    // Send notification event.
    VariantMap & d = GetEventDataMap();
    using namespace MyEvents::WorldStateChanged;

    const bool orbiting = true;

    d[ P_ORBITING ] = (void *)&orbiting;
    d[ P_PLANET_OLD ]   = (void *)planet;
    d[ P_PLANET_NEW ]   = (void *)closestP;

    SendEvent( MyEvents::E_WORLD_STATE_CHANGED, d );

    planet = SharedPtr<PlanetBase>( closestP );
}

PlanetBase * WorldMover::planetOfInfluence()
{
    if ( !gameData )
        return nullptr;

    const unsigned qty = gameData->planets.Size();
    Float maxF = -1.0;
    PlanetBase * closestP = nullptr;
    for ( unsigned i=0; i<qty; i++ )
    {
        Vector3d rel_r;
        Quaterniond rel_q;
        PlanetBase * p = gameData->planets[i];
        const bool relOk = relativePose( p, rel_r, rel_q );
        if ( !relOk )
            continue;
        const Float r = rel_r.Length();
        const Float GM = p->GM();
        const Float F = GM/(r*r);
        if ( (!closestP) || (F > maxF) )
        {
            maxF = F;
            closestP = p;
        }
    }

    return closestP;
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
    using namespace MyEvents::WorldSwitchedAssembly;

    d[ P_POS_OLD ]    = (void *)&curR;
    d[ P_VEL_OLD ]    = (void *)&curV;
    d[ P_ATM_OLD ]    = (void *)&curAtm;
    d[ P_PLANET_OLD ] = (void *)curPlanet;

    d[ P_POS_NEW ]    = (void *)&newR;
    d[ P_VEL_NEW ]    = (void *)&newV;
    d[ P_ATM_NEW ]    = (void *)&newAtm;
    d[ P_PLANET_NEW ] = (void *)newPlanet;

    SendEvent( MyEvents::E_WORLD_SWITCHED_ASSEMBLY, d );
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
