
#ifndef __PLANET_TEST_H_
#define __PLANET_TEST_H_

#include "planet_base.h"
#include "launch_site.h"

using namespace Urho3D;

namespace Osp
{

class PlanetTest: public PlanetBase
{
    URHO3D_OBJECT( PlanetTest, PlanetBase )
public:
    PlanetTest( Context * ctx );
    ~PlanetTest();

    void Start() override;

    void updateCollisionObjects( PhysicsWorld2 * w2, const Vector3d & center, const Vector3 & dist ) override;
    void initCollisions( PhysicsWorld2 * w2, const Vector3d & center,Float dist ) override;
    void finitCollisions( PhysicsWorld2 * w2 ) override;

public:
    SharedPtr<LaunchSite> site;
    // Graphical objects
    SharedPtr<Node> sphereNode;
};

}

#endif



