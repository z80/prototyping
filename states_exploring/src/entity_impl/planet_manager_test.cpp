
#include "planet_manager_test.h"
#include "entity_test_planet.h"


namespace Entity
{

PlanetManagerTest::PlanetManagerTest()
    : PlanetManagerBase()
{
}

PlanetManagerTest::~PlanetManagerTest()
{
}


void PlanetManagerTest::createImpl()
{
    // This is a "Sun".
    {
        EntityTestPlanet * p = new EntityTestPlanet();
        p->R         = 100.0;
        p->rotPeriod = 3.0;
        p->orbitPeriod = 60.0*3.0;
        p->orbitRadius = 1.0;
        p->parent = 0;
        p->create();
        p->planetName = "sun";
        planets.push_back( p );
    }
    // This is an "Earth".
    {
        EntityTestPlanet * p = new EntityTestPlanet();
        p->R         = 30.0;
        p->rotPeriod = 5.0;
        p->orbitPeriod = 60.0;
        p->orbitRadius = 300.0;
        p->parent = *(planets.rbegin());
        p->create();
        p->planetName = "earth";
        planets.push_back( p );
    }
    // This is a "Moon".
    {
        EntityTestPlanet * p = new EntityTestPlanet();
        p->R         = 10.0;
        p->rotPeriod = 3.0;
        p->orbitPeriod = 10.0;
        p->orbitRadius = 50.0;
        p->parent = *(planets.rbegin());
        p->create();
        p->planetName = "moon";
        planets.push_back( p );
    }
}



}


