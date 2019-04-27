
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

void Player::startWithAssembly( LaunchSite * site )
{
    if ( planet )
        planet->finitCollisions( physicsWorld );

    {
        Node * siteNode = site->GetNode();
        Node * playerNode = GetNode();
        playerNode->SetParent( siteNode );
    }

    PlanetBase * p = parentPlanet( n );
    if ( p )
    {
        Node * node = planet->dynamicsNode;
        Node * physicsNode = physicsWorld->GetNode();
        physicsNode->SetParent( node );

        planet = SharedPtr<Planet>( p );

        Vector3d    rel_r;
        Quaterniond rel_q;
        this->relativePose( p, rel_r, rel_q );
        p->initCollisions( physicsWorld, rel_r, 10000.0 );
    }


    if ( !gameData )
        return;


}

void Player::Start()
{
    Scene * s = GetScene();
    gameData = SharedPtr<GameData>( s->GetOrCreateComponent<GameData>() );

    Node * n = GetNode();

    PlanetBase * planet = parentPlanet( n );
    if ( planet )
    {
        Node * node = planet->dynamicsNode;
        Node * physicsNode = node->CreateChild( "PhysicsWorldNode" );
        PhysicsWorld2 * pw2 = physicsNode->CreateComponent<PhysicsWorld2>();
        physicsWorld = SharedPtr<PhysicsWorld2>( pw2 );
    }
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
    if ( physicsWorld )
        physicsWorld->DrawDebugGeometry( true );
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

    Node * pn = n->GetParent();
    if ( !pn )
        return nullptr;
    PlanetBase * p = parentPlanet( pn );
    return p;
}


}


