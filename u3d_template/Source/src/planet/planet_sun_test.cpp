
#include "planet_sun_test.h"

namespace Osp
{

PlanetSunTest::PlanetSunTest( Context * ctx )
    : PlanetBase( ctx )
{

}

PlanetSunTest::~PlanetSunTest()
{

}

void PlanetSunTest::Start()
{
    PlanetBase::Start();

    // GM, a, e, Omega, I, omega, E
    if ( mover )
        mover->Remove();
    if ( rotator )
        rotator->Remove();

    // Create graphical objects.
    {
        billboardNode = SharedPtr<Node>( dynamicsNode->CreateChild( "BillboardNode" ) );
        Node * s = billboardNode;
        BillboardSet * bb = s->CreateComponent<BillboardSet>();
        bb->SetNumBillboards( 1 );
        ResourceCache * cache = GetSubsystem<ResourceCache>();
        Material * m = cache->GetResource<Material>( "Materials/SunTest.xml" );
        bb->SetMaterial( m );

        bb->Commit();
    }
}


}

