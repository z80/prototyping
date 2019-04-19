
#include "planet_test.h"

namespace Osp
{

PlanetTest::PlanetTest( Context * ctx )
    : PlanetBase( ctx )
{

}

PlanetTest::~PlanetTest()
{

}

void PlanetTest::Start()
{
    PlanetBase::Start();

    // GM, a, e, Omega, I, omega, E
    mover->launch( 1.0, 5.0, 0.5, 0.0, 0.0 );
    rotator->launch( 3, 0.0 );

    // Create graphical objects.
    {
        sphereNode = SharedPtr<Node>( dynamicsNode->CreateChild( "SphereNode" ) );
        Node * s = sphereNode;
        StaticModel * m = s->CreateComponent<StaticModel>();

        ResourceCache * cache = GetSubsystem<ResourceCache>();
        m->SetModel( cache->GetResource<Model>( "Models/Sphere.mdl" ) );
        m->SetMaterial( cache->GetResource<Material>( "Materials/Stone.xml" ) );
        m->SetCastShadows( true );
        s->SetScale( ( 1.0, 1.0, 1.0 ) );
    }
}


}

