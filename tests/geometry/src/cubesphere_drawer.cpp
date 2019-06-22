
#include "cubesphere_drawer.h"
#include "cubesphere.h"

using namespace Cubesphere;

TestNeedSubdrive::TestNeedSubdrive()
    : NeedSubdrive()
{

}

TestNeedSubdrive::~TestNeedSubdrive()
{

}

bool TestNeedSubdrive::subdrive( const Cubesphere::Cubesphere * s, const Face * f )
{
    const bool needDivide = (f->level < 2);
    return needDivide;
}

TestSource::TestSource()
    : Source()
{

}

TestSource::~TestSource()
{

}

Float TestSource::dh( const Vector3d & at ) const
{
    return 0.0;
}




CubesphereDrawer::CubesphereDrawer( Context * c )
    : LogicComponent( c )
{
    cs = 0;
    needSubdrive = 0;
    source = 0;
    cg = 0;
}

CubesphereDrawer::~CubesphereDrawer()
{
    delete cs;
    delete needSubdrive;
    delete source;
}

void CubesphereDrawer::Start()
{
    needSubdrive = new TestNeedSubdrive();
    source       = new TestSource();
    Node * n = GetNode();
    cg = n->CreateComponent<CustomGeometry>();
}

void CubesphereDrawer::Update(float timeStep)
{

}



