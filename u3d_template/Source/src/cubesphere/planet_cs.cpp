
#include "planet_cs.h"

namespace Osp
{

PlanetCs::PlanetCs( Context * ctx )
    : PlanetBase( ctx )
{
    cg             = 0;
    rigidBody      = 0;
    collisionShape = 0;
}

PlanetCs::~PlanetCs()
{
}

void PlanetCs::Start()
{
}

void PlanetCs::updateCollisions( PhysicsWorld2 * w2, Osp::WorldMover * mover, Float dist )
{
}

void PlanetCs::initCollisions( PhysicsWorld2 * w2, Osp::WorldMover, Float dist )
{
}

void PlanetCs::finitCollisions( PhysicsWorld2 * w2 )
{
}


}




