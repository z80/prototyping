
#include "icosphere.h"

namespace IcoHeightmap
{

Real distanceL1( const Vector3 & a, const Vector3 & b )
{
    const Real x = Math::Abs( a.x - b.x );
    const Real y = Math::Abs( a.y - b.y );
    const Real z = Math::Abs( a.z - b.z );
    Real d = x + y + z;
    return d;
}


Vertex::Vertex()
{
    isMidPoint    = false;
    sourceApplied = false;
    maxLevel   = 1;
    trisQty    = 5;
    trisQty2   = 5;
    a = 0;
    b = 0;
}

Vertex::~Vertex()
{

}

Vertex::Vertex( Real x, Real y, Real z )
{
    isMidPoint    = false;
    sourceApplied = false;
    at.x = x;
    at.y = y;
    at.z = z;
    maxLevel = 1;
    trisQty  = 5;
    trisQty2 = 5;
    a = 0;
    b = 0;
}

Vertex::Vertex( const Vertex & inst )
{
    *this = inst;
}

const Vertex & Vertex::operator=( const Vertex & inst )
{
    if ( this != &inst )
    {
        at       = inst.at;
        norm     = inst.at;
        uv       = inst.uv;
        isMidPoint = inst.isMidPoint;
        sourceApplied = inst.sourceApplied;
        a        = inst.a;
        b        = inst.b;
        maxLevel = inst.maxLevel;
        trisQty  = inst.trisQty;
    }
    return *this;
}








Triangle::Triangle()
{
    for ( int32 i=0; i<3; i++ )
        vertInds[i] = -1;
    for ( int32 i=0; i<4; i++ )
        subTris[i] = -1;
    parent    = -1;
    parentInd = -1;
    level     =  1;
    leaf      =  true;
}

Triangle::~Triangle()
{

}

Triangle::Triangle( int32 n1, int32 n2, int32 n3 )
{
    vertInds[0] = n1;
    vertInds[1] = n2;
    vertInds[2] = n3;
    for ( int32 i=0; i<4; i++ )
        subTris[i] = -1;
    parent    = -1;
    parentInd = -1;
    level     =  1;
    leaf      =  true;
}

Triangle::Triangle( const Triangle & inst )
{
    *this = inst;
}

const Triangle & Triangle::operator=( const Triangle & inst )
{
    if ( this != &inst )
    {
        for ( int32 i=0; i<3; i++ )
            vertInds[i] = inst.vertInds[i];
        for ( int32 i=0; i<4; i++ )
            subTris[i] = inst.subTris[i];
        parent    = inst.parent;
        parentInd = inst.parentInd;
        level     = inst.level;
        leaf      = inst.leaf;
    }
    return *this;
}

bool Triangle::subdrive( Icosphere * s, NeedSubdrive * needSubdrive )
{
    if ( !leaf )
    {
        for ( int32 i=0; i<4; i++ )
        {
            const int32 triInd = this->subTris[i];
            Triangle tri = s->tris[triInd];
            const bool subdriveOk = tri.subdrive( s, needSubdrive );
            s->tris[triInd] = tri;
            if ( !subdriveOk )
                return false;
        }
        return true;
    }

    // Check if need subdrive.
    const bool reallyNeed = needSubdrive->subdrive( s, this );
    // If not return success.
    if ( !reallyNeed )
        return true;


    const int32 newLevel = this->level + 1;
    this->leaf = false;

    // If need subdrive subdriving recursively.
    int32 n[3];
    for ( int32 i=0; i<3; i++ )
    {
        const int32 ind0 = i;
        int32 ind1 = (i+1);
        if ( ind1 > 2 )
            ind1 = 0;
        const int32 vertInd0 = this->vertInds[ind0];
        const int32 vertInd1 = this->vertInds[ind1];
//        if ( ( (vertInd0 == 0) || (vertInd0 == 9) ) &&
//             ( (vertInd1 == 0) || (vertInd1 == 9) ) )
//        {
//            int j=0;
//        }

        const EdgeHash hashN( vertInd0, vertInd1 );
        // Check if vertex already exists.
        std::map<EdgeHash, int32>::const_iterator it = s->lookup.find( hashN );
        if ( it == s->lookup.end() )
        {
            const int32 ind = static_cast<int32>( s->verts.size() );
            s->lookup[hashN] = ind;

            // Create this vertex.
            const Vertex & v0 = s->verts[vertInd0];
            const Vertex & v1 = s->verts[vertInd1];
            Vertex v;
            v.at = v0.at + v1.at;
            v.at.normalise();
            v.norm = v.at;
            v.a    = vertInd0;
            v.b    = vertInd1;
            v.maxLevel = newLevel;
            v.trisQty  = 3;

            s->verts.push_back( v );
            n[i] = ind;
        }
        else
        {
            const int32 ind = it->second;
//            if ( ind == 15 )
//            {
//                int j = 0;
//            }
            n[i] = ind;
            // Get vertex from verts array and update it's
            // triangles qty and max level.
            Vertex & v = s->verts[ind];
            if ( v.maxLevel < newLevel )
            {
                v.maxLevel = newLevel;
                v.trisQty  = 3;
            }
            else if ( v.maxLevel == newLevel )
                v.trisQty += 3;
        }
    }
    // Process current triangle vertices and also update their
    // maxLevel and trisQty.
    // For corner vertices triangles number changes only by 1.
    for ( int32 i=0; i<3; i++ )
    {
        const int32 ind = this->vertInds[i];
//        if ( ind == 15 )
//        {
//            int j = 0;
//        }
        Vertex & v = s->verts[ind];
        if ( v.maxLevel < newLevel )
        {
            v.maxLevel = newLevel;
            v.trisQty  = 1;
        }
        else if ( v.maxLevel == newLevel )
            v.trisQty += 1;
    }

    // Create 4 child triangles.
    Triangle tri1( vertInds[0], n[0], n[2] );
    Triangle tri2( n[0], vertInds[1], n[1] );
    Triangle tri3( n[2], n[1], vertInds[2] );
    Triangle tri4( n[0], n[1], n[2] );

    tri1.level = newLevel;
    tri1.leaf  = true;
    tri1.parentInd = 0;
    tri2.level = newLevel;
    tri2.leaf  = true;
    tri2.parentInd = 1;
    tri3.level = newLevel;
    tri3.leaf  = true;
    tri3.parentInd = 2;
    tri4.level = newLevel;
    tri4.leaf  = true;
    tri4.parentInd = 3;
    const int32 triN = static_cast<int32>( s->tris.size() );
//    if ( triN == 2048 )
//    {
//        int j=0;
//    }
    int32 triBase = triN;
    s->tris.push_back( tri1 );
    s->tris.push_back( tri2 );
    s->tris.push_back( tri3 );
    s->tris.push_back( tri4 );
//    if ( newLevel == 5 )
//    {
//        int j=0;
//    }
    this->subTris[0] = triBase++;
    this->subTris[1] = triBase++;
    this->subTris[2] = triBase++;
    this->subTris[3] = triBase;

    // Recursively subdrive.
    for ( int32 i=0; i<4; i++ )
    {
        const int ind = triN + i;
        // Here it should be not a reference but a value.
        // It is due to std::vector<> reallocates it's array.
        // And due to that reference may become invalid.
        Triangle tri = s->tris[ind];
        const bool subdriveOk = tri.subdrive( s, needSubdrive );
        s->tris[ind] = tri;
        if ( !subdriveOk )
            return false;
    }

    return true;
}






EdgeHash::EdgeHash()
{
    for ( int32 i=0; i<EDGE_HASH_SZ; i++ )
        d[i] = 0;
}

EdgeHash::~EdgeHash()
{

}

EdgeHash::EdgeHash( int32 a, int32 b )
{
    uchar * pa, * pb;
    if ( a <= b )
    {
        pa = reinterpret_cast<uchar *>( &a );
        pb = reinterpret_cast<uchar *>( &b );
    }
    else
    {
        pa = reinterpret_cast<uchar *>( &b );
        pb = reinterpret_cast<uchar *>( &a );
    }
    const int32 sz = sizeof(int32);
    int32 ind = 0;
    for ( int32 i=0; i<sz; i++ )
        d[ind++] = pa[i];
    for ( int32 i=0; i<sz; i++ )
        d[ind++] = pb[i];
}

EdgeHash::EdgeHash( const EdgeHash & inst )
{
    *this = inst;
}

const EdgeHash & EdgeHash::operator=( const EdgeHash & inst )
{
    if ( this != &inst )
    {
        for ( int32 i=0; i<EDGE_HASH_SZ; i++ )
            d[i] = inst.d[i];
    }
    return *this;
}

bool operator<( const EdgeHash & a, const EdgeHash & b )
{
    for ( int32 i=0; i<EDGE_HASH_SZ; i++ )
    {
        if ( a.d[i] < b.d[i] )
            return true;
        else if ( a.d[i] > b.d[i] )
            return false;
    }
    return false;
}

bool operator==( const EdgeHash & a, const EdgeHash & b )
{
    for ( int32 i=0; i<EDGE_HASH_SZ; i++ )
    {
        if ( a.d[i] != b.d[i] )
            return false;
    }
    return true;
}








Icosphere::Icosphere()
{
    verts.reserve( 4096 );
    tris.reserve( 4096 );
    init();
}

Icosphere::~Icosphere()
{

}

Icosphere::Icosphere( const Icosphere & inst )
{
    *this = inst;
}

const Icosphere & Icosphere::operator=( const Icosphere & inst )
{
    if ( this != &inst )
    {
        verts  = inst.verts;
        tris   = inst.tris;
        lookup = inst.lookup;
    }
    return *this;
}

void Icosphere::clear()
{
    verts.clear();
    tris.clear();
    lookup.clear();

    init();
}

bool Icosphere::subdrive( NeedSubdrive * needSubdrive )
{
    const int32 qty = static_cast<int32>( tris.size() );
    for ( int32 i=0; i<qty; i++ )
    {
        Triangle tri = tris[i];
        const bool subdriveOk = tri.subdrive( this, needSubdrive );
        tris[i] = tri;
        if ( !subdriveOk )
            return false;
    }
    /*const int i = 0;
    Triangle tri = tris[i];
    const bool subdriveOk = tri.subdrive( this, needSubdrive );
    tris[i] = tri;
    if ( !subdriveOk )
        return false;*/

    labelMidPoints();
    scaleToSphere();

    return true;
}

void Icosphere::init()
{
    // Initialize with icosahedron vertices and triangles.
    const Real X=0.525731112119133606;
    const Real Z=0.850650808352039932;
    const Real N=0.0;

    /*
    static const VertexList vertices=
    {
      {-X,N,Z}, {X,N,Z}, {-X,N,-Z}, {X,N,-Z},
      {N,Z,X}, {N,Z,-X}, {N,-Z,X}, {N,-Z,-X},
      {Z,X,N}, {-Z,X, N}, {Z,-X,N}, {-Z,-X, N}
    };

    static const TriangleList triangles=
    {
      {0,4,1},{0,9,4},{9,5,4},{4,5,8},{4,8,1},
      {8,10,1},{8,3,10},{5,3,8},{5,2,3},{2,7,3},
      {7,10,3},{7,6,10},{7,11,6},{11,0,6},{0,1,6},
      {6,1,10},{9,0,11},{9,11,2},{9,2,5},{7,2,11}
    };*/
    verts.push_back( Vertex( -X,  N,  Z ) );
    verts.push_back( Vertex(  X,  N,  Z ) );
    verts.push_back( Vertex( -X,  N, -Z ) );
    verts.push_back( Vertex(  X,  N, -Z ) );

    verts.push_back( Vertex(  N,  Z,  X ) );
    verts.push_back( Vertex(  N,  Z, -X ) );
    verts.push_back( Vertex(  N, -Z,  X ) );
    verts.push_back( Vertex(  N, -Z, -X ) );

    verts.push_back( Vertex(  Z,  X,  N ) );
    verts.push_back( Vertex( -Z,  X,  N ) );
    verts.push_back( Vertex(  Z, -X,  N ) );
    verts.push_back( Vertex( -Z, -X,  N ) );

    tris.push_back( Triangle( 0, 1, 4 ) );
    tris.push_back( Triangle( 0, 4, 9 ) );
    tris.push_back( Triangle( 9, 4, 5 ) );
    tris.push_back( Triangle( 4, 8, 5 ) );
    tris.push_back( Triangle( 4, 1, 8 ) );

    tris.push_back( Triangle( 8, 1, 10 ) );
    tris.push_back( Triangle( 8, 10, 3 ) );
    tris.push_back( Triangle( 5, 8, 3 ) );
    tris.push_back( Triangle( 5, 3, 2 ) );
    tris.push_back( Triangle( 2, 3, 7 ) );

    tris.push_back( Triangle( 7, 3, 10 ) );
    tris.push_back( Triangle( 7, 10, 6 ) );
    tris.push_back( Triangle( 7, 6, 11 ) );
    tris.push_back( Triangle( 11, 6, 0 ) );
    tris.push_back( Triangle( 0, 6, 1 ) );

    tris.push_back( Triangle( 6, 10, 1 ) );
    tris.push_back( Triangle( 9, 11, 0 ) );
    tris.push_back( Triangle( 9, 2, 11 ) );
    tris.push_back( Triangle( 9, 5, 2 ) );
    tris.push_back( Triangle( 7, 11, 2 ) );
}

void Icosphere::labelMidPoints()
{
    // Debugging.
    const int32 qty = static_cast<int32>( verts.size() );
    for ( int32 i=0; i<qty; i++ )
    {
        Vertex & v = verts[i];
        v.trisQty2 = 0;
    }
    const int32 triQty = static_cast<int32>( tris.size() );
    for ( int32 i=0; i<qty; i++ )
    {
        const Triangle & tri = tris[i];
        if ( tri.leaf )
        {
            for ( int32 j=0; j<3; j++ )
            {
                Vertex & v = verts[ tri.vertInds[j] ];
                v.trisQty2 += 1;
            }
        }
    }
    for ( int32 i=0; i<qty; i++ )
    {
        Vertex & v = verts[i];
        // Actually, quantities should be at least close.
        // But right now I can see that they diverge a lot.
        if ( v.trisQty != v.trisQty2 )
        {
            v.trisQty = v.trisQty2;
        }
    }










    for ( int32 i=0; i<qty; i++ )
    {
        Vertex & v = verts[i];
        if ( v.trisQty >= 5  )
            v.isMidPoint = false;
        else
            v.isMidPoint = true;
    }
}

void Icosphere::scaleToSphere()
{
    const int32 qty = static_cast<int32>( verts.size() );
    for ( int32 i=0; i<qty; i++ )
    {
        Vertex & v = verts[i];
        if ( !v.isMidPoint )
        {
            v.at.normalise();
            v.norm = v.at;
        }
    }
}

void Icosphere::computeNormals()
{
    const int32 qty = static_cast<int32>( verts.size() );
    // First zero all normals.
    for ( int32 i=0; i<qty; i++ )
    {
        Vertex & v = verts[i];
        v.norm = Vector3::ZERO;
    }

    const int32 trisQty = static_cast<int32>( tris.size() );
    // Concatenate all normals.
    for ( int32 i=0; i<trisQty; i++ )
    {
        const Triangle & tri = tris[i];
        if ( !tri.leaf )
            continue;
        {
            const int32 vInd0 = tri.vertInds[0];
            const int32 vInd1 = tri.vertInds[1];
            const int32 vInd2 = tri.vertInds[2];
            Vertex & v0 = verts[vInd0];
            Vertex & v1 = verts[vInd1];
            Vertex & v2 = verts[vInd2];
            const Vector3 a = v1.at - v0.at;
            const Vector3 b = v2.at - v0.at;
            Vector3 n = a.crossProduct( b );
            n.normalise();
            v0.norm += n;
            v1.norm += n;
            v2.norm += n;
        }
    }

    // Normalize normals.
    for ( int32 i=0; i<qty; i++ )
    {
        Vertex & v = verts[i];
        v.norm.normalise();
    }
}

void Icosphere::applySource( Source * src )
{
    const int32 qty = static_cast<int32>( verts.size() );
    for ( int32 i=0; i<qty; i++ )
    {
        Vertex & v = verts[i];
        v.sourceApplied = false;
    }
    for ( int32 i=0; i<qty; i++ )
    {
        Vertex & v = verts[i];
        applySource( src, v );
    }

    computeNormals();
}

void Icosphere::applySource( Source * src, Vertex & v )
{
    if ( v.sourceApplied )
        return;
    if ( !v.isMidPoint )
    {
        const Real dh = src->dh( v.at );
        const Real d  = 1.0 + dh;
        v.at *= d;
        v.sourceApplied = true;
    }
    else
    {
        Vertex & a = verts[ v.a ];
        Vertex & b = verts[ v.b ];
        if ( !a.sourceApplied )
            applySource( src, a );
        if ( !b.sourceApplied )
            applySource( src, b );
        v.at = a.at + b.at;
        v.at = v.at * 0.5;
        v.sourceApplied = true;
    }

    // Debugging. Compute norm.
    const Real l = v.at.length();
    if ( l < 0.9 )
    {
        int i = 0;
    }
}





}


