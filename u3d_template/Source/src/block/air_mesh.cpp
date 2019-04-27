
#include "air_mesh.h"

namespace Osp
{

template<typename Int>
static void getTriangles( unsigned trianglesQty,
                          unsigned char * inds,
                          unsigned char * verts,
                          unsigned vertStride,
                          Vector<Triangle> & tris )
{
    tris.Clear();
    tris.Reserve( trianglesQty );

    for ( unsigned i=0; i<trianglesQty; i++ )
    {
        unsigned at = i*3*sizeof(Int);
        Int * ind = reinterpret_cast<Int *>( &inds[at] );

        Triangle t;
        for ( unsigned j=0; j<3; j++ )
        {
            Int vertInd = ind[j];
            float * v = reinterpret_cast<float *>( &verts[vertStride * vertInd] );
            const Vector3 p( v[0], v[1], v[2] );
            t.v[j] = p;
        }
        // Compute normal
        const Vector3 a = t.v[1] - t.v[0];
        const Vector3 b = t.v[2] - t.v[0];
        t.n = a.CrossProduct( b );
        t.n.Normalize();
        tris.Push( t );
    }
}

AirMesh::AirMesh()
{

}

AirMesh::~AirMesh()
{

}

bool AirMesh::init( StaticModel * m )
{
    unsigned geometriesQty = m->GetNumGeometries();
    if ( geometriesQty < 1 )
    {
        URHO3D_LOGERROR( "No geometries in a model" );
        return false;
    }

    Geometry * g = m->GetLodGeometry( 0, 0 );


    SharedArrayPtr<unsigned char> vertexData;
    SharedArrayPtr<unsigned char> indexData;
    unsigned vertexSize;
    unsigned indexSize;
    const PODVector<VertexElement> * elements;

    g->GetRawDataShared( vertexData, vertexSize, indexData, indexSize, elements );
    if ( !vertexData || !indexData || !elements || VertexBuffer::GetElementOffset(*elements, TYPE_VECTOR3, SEM_POSITION) != 0 )
    {
        URHO3D_LOGERROR( "Geometry with no or unsuitable CPU-side geometry data for triangle mesh collision" );
        return false;
    }

    unsigned indexStart = g->GetIndexStart();
    unsigned indexCount = g->GetIndexCount();

    unsigned numTriangles = indexCount / 3;
    unsigned char * triangleIndexBase = &indexData[indexStart * indexSize];
    unsigned triangleIndexStride = 3 * indexSize;
    unsigned char * vertexBase = vertexData;
    unsigned vertexStride = vertexSize;
    bool indexShort = (indexSize == sizeof(unsigned short)) ? sizeof(unsigned short) : sizeof(unsigned long);

    if ( indexShort )
        getTriangles<unsigned short>( numTriangles, triangleIndexBase, vertexBase, vertexStride, triangles );
    else
        getTriangles<unsigned long>( numTriangles, triangleIndexBase, vertexBase, vertexStride, triangles );

    return true;
}

void AirMesh::drawDebugGeometry( Node * n, DebugRenderer * debug )
{
    const Matrix3x4 m = n->GetWorldTransform();
    const size_t qty = triangles.Size();
    for ( size_t i=0; i<qty; i++ )
    {
        const Triangle & tri = triangles[i];
        Vector3 a = m*tri.v[0];
        Vector3 b = m*tri.v[1];
        Vector3 c = m*tri.v[2];
        Vector3 n = m*tri.n;

        debug->AddTriangle( a, b, c, Color::RED );
        debug->AddLine( a, a+n,      Color::BLUE );
    }
}

}

