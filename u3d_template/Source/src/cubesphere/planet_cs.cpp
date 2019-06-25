
#include "planet_cs.h"

namespace Osp
{

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

bool  PlanetCs::needSubdrive( const Cubesphere * s, const Face * f ) const
{

}

void PlanetCs::Start()
{
    PlanetBase::Start();

    Node * n = GetNode();
    cg = n->CreateComponent<CustomGeometry>();
    cg->SetDynamic( true );
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

void PlanetCs::initParameters()
{
    URHO3D_LOGINFO( "Loading config file " + configFileName );
    JSONFile json( context_ );
    FileSystem * fs = GetSubsystem<FileSystem>();
    const String stri = fs->GetProgramDir() + configFileName;
    const bool loaded = json.LoadFile( stri );
    if ( !loaded )
        URHO3D_LOGINFO( "Failed to load JSON file" );
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
        const Cubesphere::Vertex & v = faces[i];
        const Vector3 at( v.at.x_, v.at.y_, v.at.z_ );
        const Vector3 n( v.norm.x_, v.norm.y_, v.norm.z_ );

        cg->DefineVertex( at * 200.0 );
        cg->DefineColor( Color( 1.0, 0.0, 0.0 ) );
        cg->DefineNormal( n );
    }

    cg->Commit();
}


}




