
#ifndef __ICOSPHERE_H_
#define __ICOSPHERE_H_

#include "Ogre.h"

using namespace Ogre;

namespace Icosphere
{

class Icosphere;

class Vertex
{
public:
    Vector3 at;
    Vector3 norm;
    Vector2 uv;

    Vertex();
    ~Vertex();
    Vertex( const Vertex & inst );
    const Vertex & operator=( const Vertex & inst );
};

class Triangle
{
public:
    size_t vertInds[3];
    size_t subTris[4];
    size_t parent;
    size_t parentInd;
    int    level;
    bool   leaf;

    Triangle();
    ~Triangle();
    Triangle( const Triangle & inst );
    const Triangle & operator=( const Triangle & inst );
    bool subdrive( Icosphere * s );
};

static const size_t EDGE_HASH_SZ = sizeof(size_t)*2;

class EdgeHash
{
public:
    uchar d[EDGE_HASH_SZ];

    EdgeHash();
    ~EdgeHash();
    EdgeHash( const EdgeHash & inst );
    const EdgeHash & operator=( const EdgeHash & inst );
    friend bool operator<( const EdgeHash & a, const EdgeHash & b );
};

class Icosphere
{
public:
    std::vector<Vertex>        verts;
    std::vector<Triangle>      tris;
    std::map<EdgeHash, Vertex> lookup;

    Icosphere();
    ~Icosphere();
    Icosphere( const Icosphere & inst );
    const Icosphere & operator=( const Icosphere & inst );
};



}

#endif




