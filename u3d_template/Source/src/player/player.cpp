
#include "player.h"

using namespace Urho3D;

namespace Osp
{

Player::Player( Context * ctx )
    : Urho3D::LogicComponent( ctx )
{
    nearSurface = true;
}

Player::~Player()
{

}

void Player::Start()
{
    Scene * s = GetScene();
    gameData = SharedPtr<GameData>( s->GetOrCreateComponent<GameData>() );

    Node * n = GetNode();
    Node * physicsNode = n->CreateChild( "PhysicsWorldNode" );
    physicsWorld = SharedPtr<PhysicsWorld2>( physicsNode->CreateComponent<PhysicsWorld2>() );
}

void Player::DelayedStart()
{
}

void Player::Stop()
{

}

void Player::Update( float timeStep )
{
    if ( physicsWorld && gameData )
    {
        const Float dt = static_cast<Float>( gameData->dt ) * GameData::_ONE_SECOND;
        physicsWorld->Update( dt );
    }
    // Check keyboard state and generate appropriate commands.
}

void Player::FixedPostUpdate( float timeStep )
{

}

void Player::HandleMouseDown( StringHash t, VariantMap & e )
{

}

void Player::HandleMouseUp( StringHash t, VariantMap & e )
{

}

PlanetBase * Player::parentPlanet( Node * n )
{
    {
        const Vector<SharedPtr<Component> > & comps = n->GetComponents();
        unsigned qty = comps.Size();
        for ( unsigned i=0; i<qty; i++ )
        {
            Component * c = comps[i];
            PlanetBase * p = c->Cast<PlanetBase>();
            if ( p )
                return p;
        }
    }

    {
        const Vector<SharedPtr<Node> > & children = n->GetChildren();
        unsigned qty = children.Size();
        for ( unsigned i=0; i<qty; i++ )
        {
            Node * n = children[i];
            PlanetBase * p = parentPlanet( n );
            if ( p )
                return p;
        }
    }

    return nullptr;
}


}


