
#ifndef __AIR_MESH_H_
#define __AIR_MESH_H_

#include "item_base.h"

using namespace Urho3D;

namespace Osp
{

struct Triangle
{
    Vector3 v[13];
    Vector3 n;
};

class AirMesh
{
public:
    AirMesh();
    ~AirMesh();

    bool init( StaticModel * m );
    void drawDebugGeometry( Node * n, DebugRenderer * debug );

public:
    Vector<Triangle> triangles;
};

}


#endif



