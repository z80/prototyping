
#include "solar_system_test.h"


namespace Entity
{

SolarSystemTest::SolarSystemTest()
{
    create();
}

SolarSystemTest::~SolarSystemTest()
{
    destroy();
}

void SolarSystemTest::integrateKinematics( Ogre::Real t_sec )
{
    for ( std::vector<EntityTestPlanetPtr>::iterator it=planets.begin();
          it!=planets.end(); it++ )
    {
        EntityTestPlanetPtr & p = *it;
        p->integrateKinematics( t_sec );
    }
}

void SolarSystemTest::create()
{
    // This is a "Sun".
    {
        EntityTestPlanetPtr p( new EntityTestPlanet() );
        p->R         = 100.0;
        p->rotPeriod = 10.0;
        p->orbitPeriod = 60.0*3.0;
        p->orbitRadius = 1.0;
        p->parent = 0;
        p->create();
        planets.push_back( p );
    }
    // This is an "Earth".
    {
        EntityTestPlanetPtr p( new EntityTestPlanet() );
        p->R         = 30.0;
        p->rotPeriod = 60.0;
        p->orbitPeriod = 60.0*3.0;
        p->orbitRadius = 300.0;
        p->parent = (planets.rbegin())->get();
        p->create();
        planets.push_back( p );
    }
    // This is a "Moon".
    {
        EntityTestPlanetPtr p( new EntityTestPlanet() );
        p->R         = 10.0;
        p->rotPeriod = 60.0;
        p->orbitPeriod = 60.0;
        p->orbitRadius = 100.0;
        p->parent = (planets.rbegin())->get();
        p->create();
        planets.push_back( p );
    }
}

void SolarSystemTest::destroy()
{
    planets.clear();
}


}


