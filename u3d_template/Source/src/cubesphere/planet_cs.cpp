
#include "planet_cs.h"
#include "world_mover.h"

namespace Osp
{

class PlanetLoader
{
public:
    PlanetLoader() {}
    ~PlanetLoader() {}

    static bool loadGeometry( const JSONValue & v, PlanetCs * p );
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

}

bool  PlanetCs::needSubdrive( const Cubesphere::Cubesphere * s, const Cubesphere::Face * f ) const
{

}

void PlanetCs::Start()
{
    PlanetBase::Start();

    Node * n = GetNode();
    cg = n->CreateComponent<CustomGeometry>();
    cg->SetDynamic( true );

    initParameters();
}

void PlanetCs::updateCollisions( PhysicsWorld2 * w2, Osp::WorldMover * mover, Float dist )
{
}

void PlanetCs::initCollisions( PhysicsWorld2 * w2, Osp::WorldMover * mover, Float dist )
{
}

void PlanetCs::finitCollisions( PhysicsWorld2 * w2 )
{
}

bool PlanetCs::load( const JSONValue & root )
{
    if ( !PlanetBase::load( root ) )
        return false;

    {
        assert( root.Contains( "geometry" ) );
        const JSONValue & vv = root.Get( "geometry" );
        const bool ok = PlanetLoader::loadGeometry( vv, this );
        return ok;
    }
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
    mover->relativePose( rotator, rel_r, rel_q );
    rel_r.Normalize();

    setCameraAt( rel_r );
    cubesphere.subdrive( this );
    cubesphere.triangleList( tris );

    const unsigned qty = tris.Size();

    cg->Clear();
    cg->SetNumGeometries( 1 );
    cg->BeginGeometry( 0, TRIANGLE_LIST );

    for ( unsigned i=0; i<qty; i++ )
    {
        const Cubesphere::Vertex & v = tris[i];
        const Vector3 at( v.at.x_, v.at.y_, v.at.z_ );
        const Vector3 n( v.norm.x_, v.norm.y_, v.norm.z_ );

        cg->DefineVertex( at * 200.0 );
        cg->DefineColor( Color( 1.0, 0.0, 0.0 ) );
        cg->DefineNormal( n );
    }

    cg->Commit();
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
        const Float scale = v_hs.GetDouble();
    }


}



}




