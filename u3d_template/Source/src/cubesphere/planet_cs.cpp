
#include "planet_cs.h"
#include "world_mover.h"

#include "physics_world_2.h"
#include "rigid_body_2.h"
#include "collision_shape_2.h"

namespace Osp
{

class PlanetLoader
{
public:
    PlanetLoader() {}
    ~PlanetLoader() {}

    static bool loadGeometry( const JSONValue & v, PlanetCs * p );
    static bool loadKepler( const JSONValue & v, PlanetCs * p );
    static bool loadRotator( const JSONValue & v, PlanetCs * p );
};

PlanetCs::PlanetCs( Context * ctx )
    : PlanetBase( ctx )
{
    cg             = 0;
    rigidBody      = 0;
    collisionShape = 0;
    heightmap      = 0;
    colormap       = 0;
    initialized    = false;

    heightScale    = 0.03 / 255.0;
    subdivMaxLevel = 12;
    subdivMaxSine  = 0.1;

    configFileName = "Data/Planets/cubesphere.json";
}

PlanetCs::~PlanetCs()
{
}

void PlanetCs::setup( const String & json )
{
    configFileName = json;
}

Float PlanetCs::dh( const Vector3d & at ) const
{
    const Float p = Sqrt( at.x_*at.x_ + at.z_* at.z_ );
    const Float lon = Atan2( at.y_, p );
    const Float lat = Atan2( at.z_, at.x_ );
    const Float w = (Float)(heightmap->GetWidth()-1);
    const Float h = (Float)(heightmap->GetHeight()-1);
    const Float x = (lat + 180.0) / 360.0 * w;
    const Float y = (90.0 - lon) / 180.0 * h;
    const int ix = (int)x;
    const int iy = (int)y;
    const Color c = heightmap->GetPixel( x, y );
    const Float v = c.Average();
    const Float v_abs = heightScale * v;
    return v_abs;
}

bool  PlanetCs::needSubdrive( const Cubesphere::Cubesphere * s, const Cubesphere::Face * f ) const
{
    // Force some subdivision.
    //if ( f->level < 8 )
    //    return true;

    const bool maxLevelReached = (f->level >= subdivMaxLevel);
    if ( maxLevelReached )
        return false;

    Float maxSine = 0.0;
    for ( int i=0; i<4; i++ )
    {
        const int ind0 = i;
        int ind1 = i+1;
        if (ind1 > 3)
            ind1 = 0;
        Vector3d v0 = s->verts[ f->vertexInds[ind0] ].at - this->at;
        Vector3d v1 = s->verts[ f->vertexInds[ind1] ].at - this->at;
        v0.Normalize();
        v1.Normalize();
        const Vector3d cr = v0.CrossProduct( v1 );
        const Float d = std::abs( cr.Length() );
        if ( d > maxSine )
            maxSine = d;
    }
    const bool needSubdrive = ( maxSine >= subdivMaxSine );
    return needSubdrive;
}

void PlanetCs::Start()
{
    PlanetBase::Start();

    Node * n = GetNode();
    cg = n->CreateComponent<CustomGeometry>();
    cg->SetDynamic( true );

    initParameters();
    updateGeometry( Vector3d( 1.0, 0.0, 0.0 ) );
}

void PlanetCs::updateCollisions( PhysicsWorld2 * w2, Osp::WorldMover * mover, Float dist )
{
    if ( !rigidBody )
        return;

    Node * n = rigidBody->GetNode();
    Vector3d    rel_r;
    Quaterniond rel_q;
    rotator->relativePose( mover, rel_r, rel_q );
    n->SetPosition( Vector3( rel_r.x_, rel_r.y_, rel_r.z_ ) );
    n->SetRotation( Quaternion( rel_q.w_, rel_q.x_, rel_q.y_, rel_q.z_ ) );
}

void PlanetCs::initCollisions( PhysicsWorld2 * w2, Osp::WorldMover * mover, Float dist )
{
    Node * worldNode = w2->GetNode();
    Node * n = worldNode->CreateChild( "PlanetCollisionShape" );

    rigidBody = n->CreateComponent<RigidBody2>();
    rigidBody->SetMass( 0.0 );

    collisionShape = n->CreateComponent<CollisionShape2>();
    collisionShape->SetCustomGImpactMesh( cg );
    //collisionShape->SetCustomTriangleMesh( cg );

    Vector3d    rel_r;
    Quaterniond rel_q;
    rotator->relativePose( mover, rel_r, rel_q );
    n->SetPosition( Vector3( rel_r.x_, rel_r.y_, rel_r.z_ ) );
    n->SetRotation( Quaternion( rel_q.w_, rel_q.x_, rel_q.y_, rel_q.z_ ) );
}

void PlanetCs::finitCollisions( PhysicsWorld2 * w2 )
{
    if ( collisionShape )
    {
        collisionShape->Remove();
        collisionShape = 0;
    }
    if ( rigidBody )
    {
        rigidBody->Remove();
        rigidBody = 0;
    }
}

bool PlanetCs::load( const JSONValue & root )
{
    if ( !PlanetBase::load( root ) )
        return false;

    {
        assert( root.Contains( "kepler" ) );
        const JSONValue & vv = root.Get( "kepler" );
        const bool ok = PlanetLoader::loadKepler( vv, this );
    }
    {
        assert( root.Contains( "rotation" ) );
        const JSONValue & vv = root.Get( "rotation" );
        const bool ok = PlanetLoader::loadRotator( vv, this );
    }
    {
        assert( root.Contains( "geometry" ) );
        const JSONValue & vv = root.Get( "geometry" );
        const bool ok = PlanetLoader::loadGeometry( vv, this );
    }

    return true;
}

void PlanetCs::initParameters()
{
    URHO3D_LOGINFO( "Loading config file " + configFileName );
    JSONFile json( context_ );
    FileSystem * fs = GetSubsystem<FileSystem>();
    const String stri = fs->GetProgramDir() + configFileName;
    const bool loaded = json.LoadFile( stri );
    if ( !loaded )
        URHO3D_LOGINFO( "Failed to load JSON file" );
    const JSONValue & root = json.GetRoot();
    if ( !root.IsObject() )
        URHO3D_LOGINFO( "JSON file doesn't contain proper structure" );

    load( root );
}

void PlanetCs::updateGeometry( Osp::WorldMover * mover )
{
    Vector3d    rel_r;
    Quaterniond rel_q;
    if ( mover )
    {
        mover->relativePose( rotator, rel_r, rel_q );
        rel_r.Normalize();
    }

    updateGeometry( rel_r );
}

void PlanetCs::updateGeometry( const Vector3d & at )
{
    setCameraAt( at );
    cubesphere.subdrive( this );
    cubesphere.triangleList( tris );

    const unsigned qty = tris.Size();

    cg->Clear();
    cg->SetNumGeometries( 1 );
    cg->BeginGeometry( 0, TRIANGLE_LIST );

    const Float w = (Float)(heightmap->GetWidth()-1);
    const Float h = (Float)(heightmap->GetHeight()-1);
    for ( unsigned i=0; i<qty; i++ )
    {
        const Cubesphere::Vertex & v = tris[i];
        const Vector3 at( v.at.x_, v.at.y_, v.at.z_ );
        const Vector3 n( v.norm.x_, v.norm.y_, v.norm.z_ );

        const Float p = Sqrt( v.at.x_*v.at.x_ + v.at.z_* v.at.z_ );
        const Float lon = Atan2( v.at.y_, p );
        const Float lat = Atan2( v.at.z_, v.at.x_ );
        const Float x0 = (lat + 180.0) / 360.0;
        const Float y0 = (90.0 - lon) / 180.0;
        const Vector2 texCoord( x0, y0 );
        const Float x = x0 * w;
        const Float y = y0 * h;
        const int ix = (int)x;
        const int iy = (int)y;
        const Color c = colormap->GetPixel( x, y );

        cg->DefineVertex( at * forces->R_ );
        cg->DefineColor( c );
        cg->DefineNormal( n );
        cg->DefineTexCoord( texCoord );
    }

    cg->Commit();


    ResourceCache * cache = GetSubsystem<ResourceCache>();
    Material * m = cache->GetResource<Material>("Materials/VertexColor.xml");
    //Material * m = cache->GetResource<Material>("Materials/Stone.xml");
    cg->SetMaterial( m );
    cg->SetCastShadows( true );
}

bool PlanetLoader::loadGeometry( const JSONValue & v, PlanetCs * p )
{
    ResourceCache * cache = p->GetSubsystem<ResourceCache>();

    {
        if ( !v.Contains( "heightmap" ) )
            URHO3D_LOGERROR( "Planet loader error: no heightmap specified" );
        const JSONValue & v_hm = v.Get( "heightmap" );
        const String & heightmapResourceName = v_hm.GetString();
        p->heightmap = cache->GetResource<Image>( heightmapResourceName );
        assert( p->heightmap != nullptr );
    }
    {
        if ( !v.Contains( "colormap" ) )
            URHO3D_LOGERROR( "Planet loader error: no colormap specified" );
        const JSONValue & v_cm = v.Get( "colormap" );
        const String & colormapResourceName = v_cm.GetString();
        p->colormap = cache->GetResource<Image>( colormapResourceName );
        assert( p->colormap != nullptr );
    }
    {
        if ( !v.Contains( "height_scale" ) )
            URHO3D_LOGERROR( "Planet loader error: no height scale specified" );
        const JSONValue & v_hs = v.Get( "height_scale" );
        p->heightScale = v_hs.GetDouble();
    }
    {
        if ( !v.Contains( "subdiv_max_sine" ) )
            URHO3D_LOGERROR( "Planet loader error: no max subdiv sine specified" );
        const JSONValue & v_hs = v.Get( "subdiv_max_sine" );
        p->subdivMaxSine = v_hs.GetDouble();
    }
    {
        if ( !v.Contains( "subdiv_max_level" ) )
            URHO3D_LOGERROR( "Planet loader error: no max subdiv level specified" );
        const JSONValue & v_hs = v.Get( "subdiv_max_level" );
        p->subdivMaxLevel = v_hs.GetInt();
    }
    return true;
}

bool PlanetLoader::loadKepler( const JSONValue & v, PlanetCs * p )
{
    Float GM, a, e, Omega, I, omega, E;
    {
        assert( v.Contains( "GM" ) );
        const JSONValue & vv = v.Get( "GM" );
        GM = vv.GetDouble();
    }
    {
        assert( v.Contains( "a" ) );
        const JSONValue & vv = v.Get( "a" );
        a = vv.GetDouble();
    }
    {
        assert( v.Contains( "e" ) );
        const JSONValue & vv = v.Get( "e" );
        e = vv.GetDouble();
    }
    {
        assert( v.Contains( "Omega" ) );
        const JSONValue & vv = v.Get( "Omega" );
        Omega = vv.GetDouble();
    }
    {
        assert( v.Contains( "I" ) );
        const JSONValue & vv = v.Get( "I" );
        I = vv.GetDouble();
    }
    {
        assert( v.Contains( "omega" ) );
        const JSONValue & vv = v.Get( "omega" );
        omega = vv.GetDouble();
    }
    {
        assert( v.Contains( "E" ) );
        const JSONValue & vv = v.Get( "E" );
        E = vv.GetDouble();
    }
    p->mover->launch( GM, a, e, Omega, I, omega, E );
}

bool PlanetLoader::loadRotator( const JSONValue & v, PlanetCs * p )
{
    Float period, yaw, pitch, roll;
    {
        assert( v.Contains( "period" ) );
        const JSONValue & vv = v.Get( "period" );
        period = vv.GetDouble();
    }
    {
        assert( v.Contains( "yaw" ) );
        const JSONValue & vv = v.Get( "yaw" );
        yaw = vv.GetDouble();
    }
    {
        assert( v.Contains( "pitch" ) );
        const JSONValue & vv = v.Get( "pitch" );
        pitch = vv.GetDouble();
    }
    {
        assert( v.Contains( "roll" ) );
        const JSONValue & vv = v.Get( "roll" );
        roll = vv.GetDouble();
    }

    p->rotator->launch( period, yaw, pitch, roll );
    return true;
}




}




