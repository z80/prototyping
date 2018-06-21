
#ifndef __ICOSPHERE_H_
#define __ICOSPHERE_H_

#include "Ogre.h"
//#include <functional>

using namespace Ogre;

class Source;

namespace IcoHeightmap
{

class Icosphere;
class Triangle;

Real distanceL1( const Vector3 & a, const Vector3 & b );

class NeedSubdrive
{
public:
    virtual bool subdrive( const Icosphere * s, const Triangle * tri ) const = 0;
};

class Source
{
public:
    /// Delta height assuming sphere radius is 1.
    virtual Real dh( const Vector3 & at ) const = 0;
};



class Vertex
{
public:
    Vector3 at;
    Vector3 norm;
    Vector2 uv;

    // If this vertex is middle point.
    bool isMidPoint;
    // If this is middle point 3d position
    // is mean of 3d positions of the two vertices
    // with these indices.
    int32 a, b;
    // These are needed to count maxLevel triangles
    // containing this vertex.
    int maxLevel;
    int trisQty;

    Vertex();
    ~Vertex();
    Vertex( Real x, Real y, Real z );
    Vertex( const Vertex & inst );
    const Vertex & operator=( const Vertex & inst );
};

class Triangle
{
public:
    int32 vertInds[3];
    int32 subTris[4];
    int32 parent;
    int32 parentInd;
    int   level;
    bool  leaf;

    Triangle();
    ~Triangle();
    Triangle( int32 n1, int32 n2, int32 n3 );
    Triangle( const Triangle & inst );
    const Triangle & operator=( const Triangle & inst );
    bool subdrive( Icosphere * s, NeedSubdrive * needSubdrive );
};

static const int32 EDGE_HASH_SZ = sizeof(int32)*2;

class EdgeHash
{
public:
    uchar d[EDGE_HASH_SZ];

    EdgeHash();
    ~EdgeHash();
    EdgeHash( int32 a, int32 b );
    EdgeHash( const EdgeHash & inst );
    const EdgeHash & operator=( const EdgeHash & inst );
    friend bool operator<( const EdgeHash & a, const EdgeHash & b );
    friend bool operator==( const EdgeHash & a, const EdgeHash & b );
};

class Icosphere
{
public:
    std::vector<Vertex>        verts;
    std::vector<Triangle>      tris;
    std::map<EdgeHash, int32>  lookup;

    Icosphere();
    ~Icosphere();
    Icosphere( const Icosphere & inst );
    const Icosphere & operator=( const Icosphere & inst );

    void clear();
    bool subdrive( NeedSubdrive * needSubdrive );
    void init();
    void labelMidPoints();
    void scaleToSphere();
    void applySource( Source * src );
};



}

#endif




