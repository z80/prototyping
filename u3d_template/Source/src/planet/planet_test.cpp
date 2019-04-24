
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
    mover->launch( 200.0, 50.0, 0.05, 0.0, 0.0 );
    rotator->launch( 30, 0.0 );

    // Create graphical objects.
    {
        Random();
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
        const Quaternion q = Quaternion( 70.0, Vector3( 0.0, 0.0, 1.0 ) );
        const Vector3 at = q * Vector3( 0.0, 11.0, 0.0 );

        n->SetPosition( at );
        n->SetRotation( q );
        site = n->CreateComponent<LaunchSite>();

        {
            Scene * s = GetScene();
            Camera * c = s->GetComponent( StringHash( "Camera" ), true )->Cast<Camera>();
            Node * cn = c->GetNode();
            cn->SetParent( n );
        }
    }
}


}

