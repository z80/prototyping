
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

bool PlanetForces::atmosphereParams( const Vector3 & at, Float & density, Float & viscosity, Float & temperature )
{
    const Float h = at.Length() - R_;
    const Float x = h / atmHeight_;
    if ( x > 1.0 )
    {
        density_ = 0.0;
        viscosity = 0.0;
        temperature = 0.0;
        return false;
    }

    if ( h <= 0.0 )
    {
        density     =  density_;
        viscosity   = viscosity_;
        temperature = groundT_;

        return true;
    }

    const Float x2 = x*x;
    const Float _1_x2 = 1.0 - x2;
    density     = _1_x2*density_;
    viscosity   = _1_x2*viscosity_;
    temperature = groundT_*_1_x2 + highAltitudeT_*x2;
    return true;
}




}


