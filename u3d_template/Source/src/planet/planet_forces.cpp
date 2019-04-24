
#include "planet_forces.h"

namespace Osp
{

PlanetForces::PlanetForces( Context * ctx )
    : LogicComponent( ctx )
{
    GM_ = 100.0;
    R_ = 10.0;
    atmHeight_ = 7.0;
    density_ = 1.0;
    viscosity_ = 1.0;

    // Temperature parameters.
    groundT_       =  20.0;
    highAltitudeT_ = -100.0;
}

PlanetForces::~PlanetForces()
{

}

void PlanetForces::Start()
{
    Node * n = GetNode();
    const Vector<SharedPtr<Component> > & comps = n->GetComponents();
    const size_t qty = comps.Size();
    for ( size_t i=0; i<qty; i++ )
    {
        Component * c = comps[i];
        PlanetBase * p = c->Cast<PlanetBase>();
        if ( p )
        {
            planet = SharedPtr<PlanetBase>( p );
            break;
        }
    }
}

void PlanetForces::Update( float dt )
{
    if ( !planet )
        return;
    {
        const Vector<SharedPtr<Block> > & blocks = planet->surfaceBlocks;
        const size_t qty = blocks.Size();
        for ( size_t i=0; i<qty; i++ )
        {
            SharedPtr<Block> b = blocks[i];
            applyClose( b );
        }
    }
    {
        const Vector<SharedPtr<Block> > & blocks = planet->orbitingBlocks;
        const size_t qty = blocks.Size();
        for ( size_t i=0; i<qty; i++ )
        {
            SharedPtr<Block> b = blocks[i];
            applyFar( b );
        }
    }
}

void PlanetForces::applyClose( Block * b )
{
    applyGravity( b );
    applyFriction( b );
}

void PlanetForces::applyFar( Block * b )
{
    applyGravity( b );
}

void PlanetForces::applyGravity( Block * b )
{

}

void PlanetForces::applyFriction( Block * b )
{

}

Float PlanetForces::density( const Vector3d & at )
{
    return 1.0;
}

Float PlanetForces::viscosity( const Vector3d & at )
{
    return 1.0;
}

Float PlanetForces::temperature( const Vector3d & at )
{
    return 20.0;
}


}


