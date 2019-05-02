
#include "player.h"

using namespace Urho3D;

namespace Osp
{

Player::Player( Context * ctx )
    : ItemBase( ctx )
{
    nearSurface = true;
}

Player::~Player()
{

}

void Player::startWithAssembly()
{
    if ( planet )
        planet->finitCollisions( physicsWorld );

    Node * siteNode = site->GetNode();
    Node * playerNode = GetNode();
    playerNode->SetParent( siteNode );

    {
        Node * n = cameraOrb->GetNode();
        n->SetParent( siteNode );
    }

    PlanetBase * p = parentPlanet( playerNode );
    if ( p )
    {
        planet = SharedPtr<PlanetBase>( p );
        Node * node = planet->dynamicsNode;
        Node * physicsNode = physicsWorld->GetNode();
        physicsNode->SetParent( node );

        Vector3d    rel_r;
        Quaterniond rel_q;
        this->relativePose( p, rel_r, rel_q );
        p->initCollisions( physicsWorld, rel_r, 10000.0 );
    }


    if ( !gameData )
        return;

    Assembly * a = Assembly::create( siteNode, gameData->design );
    //a->toWorld( planet, physicsWorld );
    assembly = SharedPtr<Assembly>( a );

    // Focus camera on assembly.
    Node * cameraNode = cameraOrb->GetNode();
    cameraNode->SetParent( assembly->GetNode() );

    // Notify physics world mover about the new assembly.
    sendEventAssemblySelected();
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
        pw2->SetGravity( Vector3::ZERO );
        physicsWorld = SharedPtr<PhysicsWorld2>( pw2 );

        Component * c = s->GetComponent( StringHash( "LaunchSite" ), true );
        LaunchSite * l = c->Cast<LaunchSite>();
        site = SharedPtr<LaunchSite>( l );
    }

    {
        Component * c = s->GetComponent( StringHash("CameraOrb2"), true );
        CameraOrb2 * o = c->Cast<CameraOrb2>();
        cameraOrb = SharedPtr<CameraOrb2>( o );
        Node * cn = cameraOrb->GetNode();
        cn->SetParent( n );
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

void Player::sendEventAssemblySelected()
{

}


}


