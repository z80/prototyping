
#include "cubesphere_drawer.h"
#include "cubesphere.h"

using namespace Cubesphere;

TestSource::TestSource()
    : Source()
{
    at = Vector3d( 0.0, 0.0, -1.0 );
}

TestSource::~TestSource()
{

}

Float TestSource::dh( const Vector3d & at ) const
{
    return 0.0;
}

bool TestSource::needSubdrive( const Cubesphere::Cubesphere * s, const Face * f ) const
{
    const bool maxLevelNotReached = (f->level < 12);
    if ( !maxLevelNotReached )
        return false;
    /*Vector3d m = Vector3d::ZERO;
    for ( int i=0; i<4; i++ )
    {
        const Vector3d v = s->verts[ f->vertexInds[i] ].at;
        m += v;
    }
    m *= 0.25;
    m -= this->at;
    m.Normalize();*/

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
    const bool needSubdrive = ( maxSine >= 0.13 );
    return needSubdrive;

    /*Float minD = -1.0;
    for ( int i=0; i<4; i++ )
    {
        const Vector3d v = s->verts[ f->vertexInds[i] ].at - this->at;
        const Float d = v.Length();
        if ( (d < minD) || (minD < 0.0) )
            minD = d;
    }
    const bool needSubdrive = ( minD < 1.6 );

    return needSubdrive;*/
}




CubesphereDrawer::CubesphereDrawer( Context * c )
    : LogicComponent( c )
{
    cg = 0;
}

CubesphereDrawer::~CubesphereDrawer()
{
}

void CubesphereDrawer::Start()
{
    Node * n = GetNode();
    cg = n->CreateComponent<CustomGeometry>();

    cs.subdrive( &source );
    Vector<Cubesphere::Vertex> faces;
    cs.triangleList( faces );

    cg->Clear();
    cg->SetNumGeometries( 1 );
    cg->SetDynamic( true );
    //cg->BeginGeometry( 0, TRIANGLE_LIST );
    cg->BeginGeometry( 0, LINE_LIST );

    const unsigned qty = faces.Size();
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

void CubesphereDrawer::Update(float timeStep)
{

}

void CubesphereDrawer::SetMaterial( Material * m )
{
    cg->SetMaterial( 0, m );
}



