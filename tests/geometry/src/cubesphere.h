
#ifndef __CUBESPHERE_H_
#define __CUBESPHERE_H_

#include "Urho3D/Engine/Application.h"
#include "vector3d.h"

using namespace Urho3D;
using namespace Osp;

namespace Cubesphere
{

typedef double Float;

struct Vertex
{
    Vector3d v;
    int edgeIndA,
        edgeIndB;
};

struct Edge
{
    int vertexIndA,
        vertexIndB;
};

struct Face
{
    int vertexInds[4];
    int childInds[4];
    int parentInd;
    int indexInParent;
    int level;
};


}

#endif


