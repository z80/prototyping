
#ifndef __CUBESPHERE_DRAWER_H_
#define __CUBESPHERE_DRAWER_H_

#include <Urho3D/Urho3DAll.h>
#include "cubesphere.h"

using namespace Urho3D;
using namespace Cubesphere;

class TestNeedSubdrive: public NeedSubdrive
{
public:
    TestNeedSubdrive();
    ~TestNeedSubdrive();

    bool subdrive( const Cubesphere::Cubesphere * s, const Face * f ) override;
};

class TestSource: public Source
{
public:
    TestSource();
    ~TestSource();

    Float dh( const Vector3d & at ) const override;
};

class CubesphereDrawer: public Urho3D::LogicComponent
{
    URHO3D_OBJECT( CubesphereDrawer, LogicComponent )
public:
    CubesphereDrawer( Context * c );
    ~CubesphereDrawer();

    void Start() override;
    void Update(float timeStep) override;

public:
    TestNeedSubdrive * needSubdrive;
    TestSource       * source;
    Cubesphere::Cubesphere * cs;
    CustomGeometry * cg;
};

#endif


