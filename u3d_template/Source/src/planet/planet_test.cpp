
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
    mover->launch( 100.0, 20.0, 0.5, 0.0, 0.0 );
    rotator->launch( 30, 0.0 );

    // Create graphical objects.
    {
        // Sphere.
        sphereNode = SharedPtr<Node>( dynamicsNode->CreateChild( "SphereNode" ) );
        Node * s = sphereNode;
        StaticModel * m = s->CreateComponent<StaticModel>();

        ResourceCache * cache = GetSubsystem<ResourceCache>();
        m->SetModel( cache->GetResource<Model>( "Models/PlanetTest.mdl" ) );
        m->SetMaterial( cache->GetResource<Material>( "Materials/Stone.xml" ) );
        m->SetCastShadows( true );
        s->SetScale( ( 1.0, 1.0, 1.0 ) );
    }

    {
        // Launch site
        Node * n = SharedPtr<Node>( dynamicsNode->CreateChild( "LaunchSite" ) );
        n->SetPosition( Vector3( 20.0, 0.0, 0.0 ) );
        n->SetRotation( Quaternion( 90.0, Vector3( 0.0, 0.0, 1.0 ) ) );
        site = n->CreateComponent<LaunchSite>();
    }
}


}

