
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
    AirMesh( Context * ctx );
    ~AirMesh();

    bool init( Model * m );

public:
    Vector<Triangle> triangles;
};

}


#endif



