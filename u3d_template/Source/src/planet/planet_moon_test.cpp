
#include "planet_moon_test.h"

namespace Osp
{

PlanetMoonTest::PlanetMoonTest( Context * ctx )
    : PlanetBase( ctx )
{

}

PlanetMoonTest::~PlanetMoonTest()
{

}

void PlanetMoonTest::Start()
{
    PlanetBase::Start();

    // GM, a, e, Omega, I, omega, E
    mover->launch( 100.0, 15.0, 0.5, 0.0, 0.0 );
    rotator->launch( 30, 0.0 );

    // Create graphical objects.
    {
        sphereNode = SharedPtr<Node>( dynamicsNode->CreateChild( "SphereNode" ) );
        Node * s = sphereNode;
        StaticModel * m = s->CreateComponent<StaticModel>();

        ResourceCache * cache = GetSubsystem<ResourceCache>();
        m->SetModel( cache->GetResource<Model>( "Models/PlanetTest.mdl" ) );
        m->SetMaterial( cache->GetResource<Material>( "Materials/Stone.xml" ) );
        m->SetCastShadows( true );
        s->SetScale( ( 1.0, 1.0, 1.0 ) );
    }
}


}

