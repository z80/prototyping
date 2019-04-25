
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

AirMesh::AirMesh( Context * ctx )
{

}

AirMesh::~AirMesh()
{

}

bool AirMesh::init( Model * m )
{
    Geometry * g = m->GetGeometry( 0, 0 );

    unsigned geometriesQty = m->GetNumGeometries();
    if ( geometriesQty < 1 )
    {
        URHO3D_LOGERROR( "No geometries in a model" );
        return false;
    }
    unsigned lodLevelsQty  = m->GetNumGeometryLodLevels( 0 );
    if ( lodLevelsQty < 1 )
    {
        URHO3D_LOGERROR( "No LOD levels in a model" );
        return false;
    }

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

}


