
#include "world_mover.h"
#include "MyEvents.h"

namespace Osp
{

const Float WorldMover::distToMove      = 100.0;

WorldMover::WorldMover( Context * ctx )
    : KeplerMover( ctx )
{
    active = false;
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

void WorldMover::drawDebuggeometry( DebugRenderer * debug )
{
    Node * n = GetNode();
    // Draw a cube showing world center position.
    const Matrix3x4 m = n->GetWorldtransform();
    static const float d = 1.0f;
    const Vector3 a1( -d, -d,  d );
    const Vector3 b1(  d, -d,  d );
    const Vector3 c1(  d, -d, -d );
    const Vector3 d1( -d, -d, -d );
    const Vector3 a2( -d,  d,  d );
    const Vector3 v2(  d,  d,  d );
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
}

void WorldMover::switchToEvent( Assembly * assembly )
{
    // Current state.
    const Vector3d curR          = relR();
    const Vector3d curV          = relV();
    const bool     curAtm        = inAtmosphere;
    const PlanetBase * curPlanet = planet;

    // New state.
    const Vector3d newR          = assembly->relR();
    const Vector3d newV          = assembly->relV();
    const bool     newAtm        = assembly->inAtmosphere;
    const PlanetBase * newPlanet = assembly->planet;

    VariantMap & d = GetEventDataMap();
    using namespace MyEvents::WorldSwitched;

    d[ P_POS_OLD ]    = (void *)&curR;
    d[ P_VEL_OLD ]    = (void *)&curV;
    d[ P_ATM_OLD ]    = (void *)&curAtm;
    d[ P_PLANET_OLD ] = (void *)&curPlanet;

    d[ P_POS_NEW ]    = (void *)&newR;
    d[ P_VEL_NEW ]    = (void *)&newV;
    d[ P_ATM_NEW ]    = (void *)&newAtm;
    d[ P_PLANET_NEW ] = (void *)&newPlanet;

    SendEvent( MyEvents::E_WORLD_MOVED, d );
}

void WorldMover::switchTo( Assembly * assembly )
{
    // Set position to be the same.
    const Vector3d r = assembly->relR();
    const bool inAtmosphere = assembly->inAtmosphere;
    // Orbital movement is active
    active = !inAtmosphere;

    Node * selfNode = GetNode();
    PlanetBase * planet = assembly->planet;
    if ( inAtmosphere )
    {
        Node * planetNode = planet->rotator->GetNode();
        selfNode->SetParent( planetNode );
        setR( r );
    }
    else
    {
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

    const Vector3d r0 = relR();
    const Vector3d r1 = assembly->relR();
    const Vector3d dr = r0 - r1;
    const Float d = dr.Length();
    if ( (d < distToMove) && (inAtmosphere == assembly->inAtmosphere) )
        return;

    Vector3d dv;
    const Vector3d v0 = relV();
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

    moveToEvent( r1, dr, dv );
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
