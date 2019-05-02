
#include "assembly.h"
#include "name_generator.h"
//#include "Urho3D/Physics/RigidBody.h"
//#include "Urho3D/Physics/Constraint.h"
//#include "Urho3D/Physics/PhysicsWorld.h"
#include "rigid_body_2.h"
#include "constraint_2.h"
#include "physics_world_2.h"
#include "planet_base.h"
#include "game_data.h"
#include "world_mover.h"

#include "MyEvents.h"

namespace Osp
{

const Float Assembly::DIST_LEAVE_WORLD = 1500.0;
const Float Assembly::DIST_ENTER_WORLD = 1000.0;


Assembly::Assembly( Context * c )
    : ItemBase( c )
{
    inAtmosphere = true;
    onSurface    = true;
}

Assembly::~Assembly()
{
    destroy();
}

Assembly * Assembly::create( Node * root, const Design & d )
{
    if ( !root )
        return nullptr;

    Node * node = root->CreateChild( NameGenerator::Next( "Assembly" ) );

    Assembly * a = node->CreateComponent<Assembly>();
    a->design = d;
    // Create blocks.
    {
        const size_t blocksQty = d.blocks.size();
        for ( size_t i=0; i<blocksQty; i++ )
        {
            const Design::Block & db = d.blocks[i];
            const String & typeName = db.typeName;
            Node * n = node->CreateChild( NameGenerator::Next( typeName ) );
            Component * c = n->CreateComponent( StringHash( typeName ) );
            Block * b = c->Cast<Block>();
            if ( !b )
                continue;
            b->setQ( db.q );
            b->setR( db.r );
            SharedPtr<Block> sb( b );
            a->blocks.Push( sb );
        }
    }

    return a;
}

bool Assembly::toWorld( PlanetBase * planet, PhysicsWorld2 * world )
{
    PhysicsWorld2 * w = world;
    if ( !w )
        return false;
    Node * worldNode = w->GetNode();

    // Traverse all blocks and joints and add those to world.
    {
        const size_t qty = blocks.Size();
        for ( size_t i=0; i<qty; i++ )
        {
            Block * b = blocks[i];
            if ( !b )
                continue;
            // Add block to world. It should create and initialize
            // rigid body and collision shape.
            //b->toWorld();
            Node * blockNode = b->GetNode();

            // Just to check that relative pose is correct.
            //Vector3d rel_r;
            //Quaterniond rel_q;
            //b->relativePose( worldNode, rel_r, rel_q );

            blockNode->SetParent( worldNode );
            planet->addSurfaceBlock( b );
        }
    }
    {
        // Create joints.
        Design & d = design;
        const size_t connsQty = d.joints.size();
        for ( size_t i=0; i<connsQty; i++ )
        {
            const Design::Joint & dj = d.joints[i];
            const int indA = dj.blockA;
            const int indB = dj.blockB;

            SharedPtr<Block> blockA = this->blocks[indA];
            SharedPtr<Block> blockB = this->blocks[indB];

            // For now place joint point in the middle.
            const Vector3d r = blockB->relR();
            Node * node = blockB->GetNode();
            Constraint2 * c = node->CreateComponent<Constraint2>();
            c->SetConstraintType( CONSTRAINT_HINGE_2 );
            c->SetDisableCollision( true );
            c->SetOtherBody( blockA->rigidBody() );
            c->SetWorldPosition( Vector3( r.x_, r.y_, r.z_ ) );

            const Vector3 axis( 0.0, 0.0, 1.0 );
            const Vector2 lim( 0.0, 0.0 );
            c->SetAxis( axis );
            c->SetOtherAxis( axis );
            c->SetHighLimit( lim );
            c->SetLowLimit( lim );

            this->joints.Push( SharedPtr<Constraint2>( c ) );
        }
    }

    Node * assemblyNode = GetNode();
    assemblyNode->SetParent( worldNode );

    return true;
}

void Assembly::fromWorld( PlanetBase * planet )
{
    Node * root = GetNode();
    // Traverse all blocks and joints and add those to world.
    {
        const size_t qty = blocks.Size();
        for ( size_t i=0; i<qty; i++ )
        {
            Block * b = blocks[i];
            if ( !b )
                continue;
            // Remove block from world. It should destroy
            // all dynamics related objects (rigid body and collision shape).
            b->fromWorld();
            Node * bn = b->GetNode();
            bn->SetParent( root );
        }
    }
    {
        const size_t qty = joints.Size();
        for ( size_t i=0; i<qty; i++ )
        {
            Constraint2 * c = joints[i];
            if ( !c )
                continue;
            c->Remove();
        }
        joints.Clear();
    }
}

void Assembly::drawDebugGeometry( DebugRenderer * debug )
{
    unsigned qty = blocks.Size();
    for ( unsigned i=0; i<qty; i++ )
    {
        SharedPtr<Block> b = blocks[i];
        b->drawDebugGeometry( debug );
    }
}

void Assembly::Start()
{
    Node * n = GetNode();
    KeplerMover * m = n->CreateComponent<KeplerMover>();
    mover = SharedPtr<KeplerMover>( m );
    mover->active = false;

    Scene * s = GetScene();
    Component * c = s->GetComponent( StringHash( "WorldMover" ), true );
    if ( c )
    {
        ItemBase * w = c->Cast<ItemBase>();
        worldMover = SharedPtr<ItemBase>( w );
    }
}

void Assembly::Update( float timeStep )
{

}

void Assembly::PostUpdate( float timeStep )
{
    // Here recompute position and orientation.
    if ( inWorld )
        updatePoseInWorld();
    else
        updatePoseInOrbit();

    // Check if time to leave/enter world.
    if ( inWorld )
    {
        const bool leave = needLeaveWorld();
        if ( leave )
            fromWorld( planet );
    }
    else
    {
        const bool enter = needEnterWorld();
        if ( enter )
            ; //toWorld();
    }

    // Check if need to change planet orbiting around.
    if ( !inWorld )
    {
        PlanetBase * p = planetOfInfluence();
        if ( p != planet )
        {

        }
    }

    // Draw debug geometry.
    Scene * s = GetScene();
    DebugRenderer * debug = s->GetOrCreateComponent<DebugRenderer>();
    if ( debug )
        drawDebugGeometry( debug );
}

void Assembly::destroy()
{
    {
        const size_t qty = blocks.Size();
        for ( size_t i=0; i<qty; i++ )
        {
            Block * b = blocks[i];
            if ( b )
                b->Remove();
        }
    }

    {
        const size_t qty = joints.Size();
        for ( size_t i=0; i<qty; i++ )
        {
            Constraint2 * c = joints[i];
            if ( c )
                c->Remove();
        }
    }
}

void Assembly::updatePoseInWorld()
{
    if ( !inWorld )
        return;

    // Retrieve all the blocks.
    Vector3d r( Vector3d::ZERO );
    Vector3d v( Vector3d::ZERO );
    const size_t qty = blocks.Size();
    for ( size_t i=0; i<qty; i++ )
    {
        SharedPtr<Block> sb = blocks[i];
        if ( !sb )
        {
            URHO3D_LOGERROR( "Assembly::updatePoseInWorld() one of blocks doesn\'t exist!" );
            continue;
        }
        const Vector3d ri = sb->relR();
        const Vector3d vi = sb->relV();
        r += ri;
        v += vi;
    }
    const float _1_qty = 1.0/static_cast<float>( qty );
    r = r * _1_qty;
    v = v * _1_qty;
    //Node * n = GetNode();
    //n->SetPosition( Vector3( r.x_, r.y_, r.z_ ) );

    setR( r );
    setV( v );
}

void Assembly::updatePoseInOrbit()
{
    if ( !mover )
        return;
    const Vector3d r = mover->relR();
    const Vector3d v = mover->relV();

    setR( r );
    setV( v );
}

void Assembly::checkInfluence()
{
    if ( inAtmosphere || onSurface )
        return;

    const PlanetBase * closestP = planetOfInfluence();
    if ( planet == closestP )
        return;

    // Compute relative pose and velocity.
}

PlanetBase * Assembly::planetOfInfluence()
{
    if ( !gameData )
        return nullptr;

    const unsigned qty = gameData->planets.Size();
    Float maxF = -1.0;
    PlanetBase * closestP = nullptr;
    for ( unsigned i=0; i<qty; i++ )
    {
        Vector3d rel_r;
        Quaterniond rel_q;
        PlanetBase * p = gameData->planets[i];
        const bool relOk = relativePose( p, rel_r, rel_q );
        if ( !relOk )
            continue;
        const Float r = rel_r.Length();
        const Float GM = p->GM();
        const Float F = GM/(r*r);
        if ( (!closestP) || (F > maxF) )
        {
            maxF = F;
            closestP = p;
        }
    }

    return closestP;
}

bool Assembly::needLeaveWorld()
{
    if ( !inWorld )
        return false;

    Vector3d rel_r;
    Quaterniond rel_q;
    relativePose( worldMover, rel_r, rel_q );
    const Float d = rel_r.Length();
    return ( d > DIST_LEAVE_WORLD );
}

bool Assembly::needEnterWorld()
{
    if ( inWorld )
        return false;

    Vector3d rel_r;
    Quaterniond rel_q;
    relativePose( worldMover, rel_r, rel_q );
    const Float d = rel_r.Length();
    return ( d < DIST_ENTER_WORLD );
}

void Assembly::subscribeToEvents()
{
    SubscribeToEvent( MyEvents::E_WORLD_SWITCHED, URHO3D_HANDLER( Assembly, OnWorldSwitched ) );
    SubscribeToEvent( MyEvents::E_WORLD_MOVED,    URHO3D_HANDLER( Assembly, OnWorldMoved ) );
}

void Assembly::OnWorldSwitched( StringHash eventType, VariantMap & eventData )
{
    // Check first if need to leave the world.
    const bool needLeave = needLeaveWorld();

    using namespace MyEvents::WorldSwitched;
    const Variant & pl = eventdata[ P_PLANET_OLD ];
    const PlanetBase * p = (PlanetBase *)pl.GetVoidPtr();
    // Also need old world position and velocity to properly initialize movement.
    fromWorld( p );
}

void Assembly::OnWorldMoved( StringHash eventType, VariantMap & eventData )
{
    if ( !inWorld )
        return;

    using namespace MyEvents::WorldMoved;
    const Variant & varR = eventData[ P_POS_ADJ ];
    const Vector3d dr = *(Vector3d *)(varR.GetVoidPtr());
    const Variant & varV = eventData[ P_VEL_ADJ ];
    const Vector3d dv = *(Vector3d *)(varV.GetVoidPtr());
    adjustMovementInWorld( dr, dv );
}

void Assembly::adjustMovementInWorld( const Vector3 & dr, const Vector3 & dv )
{
    const Vector3d r0 = relR();
    const Vector3d v0 = relV();
    const Vector3d r1 = r0 + dr;
    const Vector3d v1 = v0 + dv;
    setR( r1 );
    setV( v1 );
}




PhysicsWorld2 * Assembly::getWorld( Node * node )
{
    Node * pn = node->GetParent();
    if ( !pn )
        return nullptr;
    PhysicsWorld2 * w = pn->GetComponent<PhysicsWorld2>();
    return w;
}


}

