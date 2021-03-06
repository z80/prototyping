
#include "entity_part.h"
#include "entity_planet.h"
#include "assembly.h"
#include "entity_world.h"
#include "dynamics_world.h"
#include "state_manager.h"
#include "part_manager_base.h"

namespace Osp
{

static void deleteSubshapes( btCollisionShape * s );

BlockConnection::BlockConnection()
{
    blockA = 0;
    blockB = 0;
    assembly    = 0;
    assemblyInd = -1;
    world       = 0;
    constraint  = 0;
}

BlockConnection::~BlockConnection()
{
    destroy();
}

void BlockConnection::toWorld( DynamicsWorld * w )
{
    fromWorld();

    w->phyWorld->addConstraint( constraint, true );
    world = w;
}

void BlockConnection::fromWorld()
{
    if ( world )
        world->phyWorld->removeConstraint( constraint );
    world = 0;
}


void BlockConnection::destroy()
{
    if ( world )
    {
        if ( constraint )
            world->phyWorld->removeConstraint( constraint );
    }
    if ( constraint )
        delete constraint;
}






Block::Block()
    : Entity()
{
    mType = TBlock;

    forcesWereApplied = false;

    actionGroup       = 0;
    isSeleted         = false;
    doShowContextMenu = false;

    visualEntity   = 0;
    visualNode     = 0;

    world          = 0;
    rigidBody      = 0;
    collisionShape = 0;
    bodyState      = 0;

    assembly    = 0;
    assemblyInd = -1;
}

Block::~Block()
{
    /*
            Globals::phyWorld->removeRigidBody(mNinjaBody);
            delete mNinjaBody->getMotionState();
            delete mNinjaBody;
            delete mNinjaShape;
    */

    deleteRigidBody();
    deleteCollisionShape();
    deleteVisual();

    StateManager::getSingletonPtr()->getPartsManager()->remove( this );
}

void Block::loadResources()
{

}

void Block::setActionGroup( Ogre::int32 group )
{
    actionGroup = group;
}

void Block::action( Ogre::int32 actionGroup, const Ogre::String & name, Ogre::Real value )
{
    // By default do nothing.
}

void Block::drawGui()
{
    const ImVec2 wndSz( 340, 120 );
    ImGui::SetNextWindowBgAlpha( 0.3f ); // Transparent background
    ImGui::SetNextWindowSizeConstraints( wndSz, wndSz );
    if ( ImGui::Begin( "Choose", 0,
                        ImGuiWindowFlags_NoResize |
                        ImGuiWindowFlags_AlwaysAutoResize |
                        ImGuiWindowFlags_NoSavedSettings )
       )
    {
        ImGui::Text( "Part GUI" );
        ImGui::Button( "Start/continue game", ImVec2( 320, 30 ) );
        ImGui::Button( "Exit to OS",          ImVec2( 320, 30 ) );
    }
    ImGui::End();
}

void Block::drawContextMenu()
{

}

void Block::computeAirFriction()
{
    //airMesh.
}

void Block::selectEvent()
{

}

void Block::unselectEvent()
{

}

void Block::contextMenuEvent()
{

}

void Block::toWorld( DynamicsWorld * w )
{
    fromWorld();
    w->phyWorld->addRigidBody( rigidBody );
    world = w;
}

void Block::fromWorld()
{
    if (!world)
        return;
    world->phyWorld->removeRigidBody( rigidBody );
    world = 0;
}

Ogre::Vector3 Block::relV() const
{
    Ogre::Vector3 v;
    if ( !rigidBody )
    {
        v.x = 0.0;
        v.y = 0.0;
        v.z = 0.0;
        return v;
    }

    const btVector3 vel = rigidBody->getLinearVelocity();
    v.x = vel.x();
    v.y = vel.y();
    v.z = vel.z();
    return v;
}

void Block::relV( Ogre::Real & v ) const
{
    if ( !rigidBody )
    {
        v = 0.0;
        return;
    }

    const btVector3  vel = rigidBody->getLinearVelocity();
    v = vel.length();
}

void Block::setV( const Ogre::Vector3 & v )
{
    btVector3 vel( v.x, v.y, v.z );
    rigidBody->setLinearVelocity( vel );
}

Ogre::Vector3 Block::relW() const
{
    const btVector3 wb = rigidBody->getAngularVelocity();
    const Ogre::Vector3 wv( wb.x(), wb.y(), wb.z() );
    return wv;
}

void Block::setW( const Ogre::Vector3 & w )
{
    rigidBody->setAngularVelocity( btVector3( w.x, w.y, w.z ) );
}





bool Block::setEntity( const char * mesh, const char * material )
{
    deleteVisual();

    Ogre::SceneManager * scnMgr = StateManager::getSingletonPtr()->getSceneManager();
    if ( !scnMgr )
        return false;
    if ( visualEntity )
        scnMgr->destroyEntity( visualEntity );

    bool res = true;
    try
    {
        visualEntity = scnMgr->createEntity( NameGenerator::Next("entity"), mesh );
        Ogre::UserObjectBindings & uob = visualEntity->getUserObjectBindings();
        uob.setUserAny( Ogre::Any( this ) );
        if ( material )
        {
            Ogre::MaterialPtr m = Ogre::MaterialManager::getSingletonPtr()->getByName( material );
            visualEntity->setMaterial( m );
        }
    }
    catch ( ... )
    {
        res = false;
    }
    return res;
}

bool Block::setMaterial( const char * material )
{
    if ( !visualEntity )
        return false;
    bool res;
    try
    {
        Ogre::MaterialPtr m = Ogre::MaterialManager::getSingletonPtr()->getByName( material );
        visualEntity->setMaterial( m );
        res = true;
    }
    catch ( ... )
    {
        res = false;
    }
    return res;
}

bool Block::setMask( Ogre::uint32 mask )
{
    if ( !visualEntity )
        return false;

    visualEntity->setQueryFlags( mask );
    return true;
}

bool Block::setAirMesh( const std::string & meshFileName )
{
    bool res = true;
    try
    {
        Ogre::MeshPtr m = Ogre::Root::getSingletonPtr()->getMeshManager()->getByName( meshFileName );
        AirMesh::AirMesh::airMesh( m, airMesh );
    }
    catch ( ... )
    {
        res = false;
    }

    return res;
}

void Block::setMass( Ogre::Real & m )
{
    mass = m;
}

void Block::setInertia( const Ogre::Vector3 & i )
{
    inertia = i;
}

void Block::setCollisionShape( btCollisionShape * shape )
{
    deleteCollisionShape();
    collisionShape = shape;
}

void Block::initDynamics()
{
    deleteRigidBody();

    bodyState      = new BtOgre::RigidBodyState( sceneNode );
    bodyState->setWorldTransform( btTransform( btQuaternion( 0.0, 0.0, 0.0, 1.0 ),
                                               btVector3( 0.0, 0.0, 0.0 ) ) );

    //Calculate inertia.
    const btScalar mass = 0.0;
    btVector3      inertia( 0.0, 0.0, 0.0 );

    rigidBody = new btRigidBody( mass, bodyState, collisionShape, inertia );

    EntityWorld::getSingletonPtr()->addEntity( this );
}

bool Block::forcesApplied() const
{
    return forcesWereApplied;
}

void Block::resetForcesApplied()
{
    forcesWereApplied = false;
}

void Block::applyForce( const Ogre::Vector3 & f, const Ogre::Vector3 & at, bool localRf )
{
    forcesWereApplied = true;
    if ( localRf )
    {
        const Ogre::Quaternion q = relQ();
        Ogre::Quaternion rq( 0.0, at.x, at.y, at.z );
        rq = q * rq * q.Inverse();
        rigidBody->applyForce( btVector3(rq.x, rq.y, rq.z), btVector3( at.x, at.y, at.z ) );
    }
    else
        rigidBody->applyForce( btVector3(f.x, f.y, f.z), btVector3( at.x, at.y, at.z ) );
}

void Block::applyTorque( const Ogre::Vector3 & p )
{
    forcesWereApplied = true;
    rigidBody->applyTorque( btVector3( p.x, p.y, p.z ) );
}

void Block::setR( const Ogre::Vector3 & at )
{
    btMotionState * st = rigidBody->getMotionState();
    btTransform t;
    st->getWorldTransform( t );
    t.setOrigin( btVector3( at.x, at.y, at.z ) );
    st->setWorldTransform( t );
}

Ogre::Vector3 Block::relR() const
{
    btMotionState * st = rigidBody->getMotionState();
    btTransform t;
    st->getWorldTransform( t );
    const btVector3 at = t.getOrigin();
    const Ogre::Vector3 r( at.x(), at.y(), at.z() );
    return r;
}

void Block::setQ( const Ogre::Quaternion & q )
{
    btMotionState * st = rigidBody->getMotionState();
    btTransform t;
    st->getWorldTransform( t );
    t.setRotation( btQuaternion( q.x, q.y, q.z, q.w ) );
    st->setWorldTransform( t );
}

Ogre::Quaternion Block::relQ() const
{
    btMotionState * st = rigidBody->getMotionState();
    btTransform t;
    st->getWorldTransform( t );
    const btQuaternion bq = t.getRotation();
    const Ogre::Quaternion q( bq.w(), bq.x(), bq.y(), bq.z() );
    return q;
}

/*void EntityPart::setParent( EntityPlanet * planet )
{
    if ( !sceneNode )
        return;

    sceneNode->setInheritOrientation( false );
    sceneNode->setInheritScale( false );

    Ogre::Vector3    r = absoluteR();
    Ogre::Quaternion q = absoluteQ();
    Ogre::Vector3    v = absoluteV();
    Ogre::Vector3    w = absoluteW();

    if ( parent!=planet )
    {
        sceneNode->getParentSceneNode()->removeChild( sceneNode );
        if ( parent )
        {
            const Ogre::Vector3    parentR = parent->absoluteR();
            const Ogre::Quaternion parentQ = parent->absoluteQ();
            const Ogre::Vector3    parentV = parent->absoluteV();
            // Due to not near surface, "w" is not added up.
            const Ogre::Vector3    parentW = parent->absoluteW();

            // Convert values to global ref. frame.
            r = parentR + r;
            if ( nearSurface )
                q = parentQ * q;
            v = parentV + v;
            if ( nearSurface )
                w = parentW + w;

        }

        if ( planet )
        {
            const Ogre::Vector3    parentR = parent->absoluteR();
            const Ogre::Quaternion parentQ = parent->absoluteQ();
            const Ogre::Vector3    parentV = parent->absoluteV();

            // Convert to new parent's ref. frame.
            r = r - parentR;
            v = v - parentV;

            planet->sceneNode->addChild( sceneNode );
        }
        else
        {
            Ogre::SceneManager * scnMgr = StateManager::getSingletonPtr()->getSceneManager();
            scnMgr->getRootSceneNode()->addChild( sceneNode );
        }
    }

    // Update position and movement parameters.
    setR( r );
    setQ( q );
    setV( v );
    setW( w );

    // Update condition variables.
    parent      = planet;
    nearSurface = false;
}

void EntityPart::setParentRf( EntityPlanet * planet )
{
    if ( !sceneNode )
        return;

    sceneNode->setInheritOrientation( true );
    sceneNode->setInheritScale( false );

    Ogre::Vector3    r = absoluteR();
    Ogre::Quaternion q = absoluteQ();
    Ogre::Vector3    v = absoluteV();
    Ogre::Vector3    w = absoluteW();

    if ( parent!=planet )
    {
        sceneNode->getParentSceneNode()->removeChild( sceneNode );
        if ( parent )
        {
            const Ogre::Vector3    parentR = parent->absoluteR();
            const Ogre::Quaternion parentQ = parent->absoluteQ();
            const Ogre::Vector3    parentV = parent->absoluteV();
            // Due to not near surface, "w" is not added up.
            const Ogre::Vector3    parentW = parent->absoluteW();

            // Convert values to global ref. frame.
            r = parentR + r;
            if ( nearSurface )
                q = parentQ * q;
            v = parentV + v;
            if ( nearSurface )
                w = parentW + w;

        }

        if ( planet )
        {
            const Ogre::Vector3    parentR = parent->absoluteR();
            const Ogre::Quaternion parentQ = parent->absoluteQ();
            const Ogre::Vector3    parentV = parent->absoluteV();
            const Ogre::Vector3    parentW = parent->absoluteW();

            // Convert to new parent's ref. frame.
            r = r - parentR;
            q = parentQ.Inverse() * q;
            v = v - parentV;
            w = w - parentW;

            planet->sceneNode->addChild( sceneNode );
        }
        else
        {
            Ogre::SceneManager * scnMgr = StateManager::getSingletonPtr()->getSceneManager();
            scnMgr->getRootSceneNode()->addChild( sceneNode );
        }
    }

    // Update position and movement parameters.
    setR( r );
    setQ( q );
    setV( v );
    setW( w );

    // Update condition variables.
    parent      = planet;
    if ( planet )
        nearSurface = true;
}
*/

bool Block::addSound( const std::string & fileName, const std::string & name )
{
    Ogre::SceneManager * scnMgr = StateManager::getSingletonPtr()->getSceneManager();
    OgreOggSound::OgreOggSoundManager * m = OgreOggSound::OgreOggSoundManager::getSingletonPtr();
    OgreOggSound::OgreOggISound * s;
    bool res = false;
    try
    {
        s = m->createSound( "name", "fileName", false, false, false, scnMgr, false );
        res = true;
    }
    catch ( ... )
    {
        s = m->getSound( "name" );
        res = true;
    }
    if ( res )
    {
        Sound ss;
        ss.name = name;
        ss.s    = s;
        sounds.push_back( ss );
        std::push_heap( sounds.begin(), sounds.end(), Sound::Lt );
    }
    return true;
}

void Block::finalizeSounds()
{
    std::sort_heap( sounds.begin(), sounds.end(), Sound::Lt );
}

bool Block::playSound( const std::string & name, bool looped )
{
    std::vector<Sound>::iterator it = std::lower_bound( sounds.begin(), sounds.end(), name, Sound::Lt );
    if ( it == sounds.end() )
        return false;
    Sound & ss = *it;
    OgreOggSound::OgreOggISound * s = ss.s;
    s->play( true );

    return true;
}

bool Block::stopSound( const std::string & name )
{
    std::vector<Sound>::iterator it = std::lower_bound( sounds.begin(), sounds.end(), name, Sound::Lt );
    if ( it == sounds.end() )
        return false;
    Sound & ss = *it;
    OgreOggSound::OgreOggISound * s = ss.s;
    s->stop( true );

    return true;
}

Ogre::Vector3    Block::absoluteV() const
{
    EntityPlanet * planet = world->planet();
    if ( !planet )
        return Ogre::Vector3::ZERO;
    const bool nearSurface = ( world == planet->worldClose );
    Ogre::Vector3 v = relV();
    const Ogre::Vector3 v_par = planet->absoluteV();
    if ( nearSurface )
    {
        // First convert speed to global ref. frame and add
        // rotational speed in that point.
        const Ogre::Quaternion q_par = planet->absoluteQ();
        Ogre::Quaternion vq( 0.0, v.x, v.y, v.z );
        vq = q_par * vq * q_par.Inverse();
        v = Ogre::Vector3( vq.x, vq.y, vq.z );

        const Ogre::Vector3 at = relR();
        Ogre::Vector3 v_par_at = planet->rotV( at, true );
        v = v_par_at + v;
    }
    v = v_par + v;
    return v;
}

Ogre::Vector3    Block::absoluteR() const
{
    EntityPlanet * planet = world->planet();
    if ( !planet )
        return Ogre::Vector3::ZERO;
    const bool nearSurface = ( world == planet->worldClose );

    Ogre::Vector3 r = relR();

    const Ogre::Vector3 r_par = planet->absoluteR();
    if ( nearSurface )
    {
        const Ogre::Quaternion q_par = planet->absoluteQ();
        Ogre::Quaternion rq( 0.0, r.x, r.y, r.z );
        rq = q_par * rq * q_par.Inverse();
        r = Ogre::Vector3( rq.x, rq.y, rq.z );
    }
    r = r_par + r;
    return r;
}

Ogre::Vector3    Block::absoluteW() const
{
    EntityPlanet * planet = world->planet();
    if ( !planet )
        return Ogre::Vector3::ZERO;
    const bool nearSurface = ( world == planet->worldClose );
    Ogre::Vector3 w = relW();

    if ( nearSurface )
    {
        const Ogre::Vector3 w_parent    = planet->absoluteW();
        const Ogre::Quaternion q_parent = planet->absoluteQ();
        Ogre::Quaternion wq( 0.0, w.x, w.y, w.z );
        wq = q_parent * wq * q_parent.Inverse();
        w = Ogre::Vector3( wq.x, wq.y, wq.z );
        w = w_parent + w;
    }
    return w;
}

Ogre::Quaternion Block::absoluteQ() const
{
    EntityPlanet * planet = world->planet();
    if ( !planet )
        return Ogre::Quaternion::IDENTITY;
    const bool nearSurface = ( world == planet->worldClose );

    Ogre::Quaternion q = relQ();
    if ( nearSurface )
    {
        const Ogre::Quaternion q_parent = planet->absoluteQ();
        q = q_parent * q;
    }
    return q;
}


void Block::deleteRigidBody()
{
    if ( rigidBody )
    {
        DynamicsWorld * w = world;
        if ( w && w->phyWorld )
            w->phyWorld->removeRigidBody( rigidBody );
        if ( bodyState )
            delete bodyState;

        delete rigidBody;

        rigidBody = 0;
        bodyState = 0;
    }
}

void Block::deleteCollisionShape()
{
    if ( collisionShape )
    {
        const int type = collisionShape->getShapeType();
        if ( type == COMPOUND_SHAPE_PROXYTYPE )
            deleteSubshapes( collisionShape );
        delete collisionShape;
        collisionShape = 0;
    }
}

void Block::deleteVisual()
{
    Ogre::SceneManager * smgr = StateManager::getSingletonPtr()->getSceneManager();
    if ( smgr )
    {
        if ( visualEntity );
        {
            smgr->destroyEntity( visualEntity );
            visualEntity = 0;
        }

        if ( visualNode )
        {
            if ( !smgr->hasSceneNode( visualNode->getName() ) )
                smgr->destroySceneNode( visualNode );
            visualNode = 0;
        }
    }
}

void Block::toAssembly()
{

}

void Block::fromAssembly()
{
    const Ogre::Quaternion assQ = assembly->relQ();
    const Ogre::Quaternion q = assQ * assemblyQ;
    const Ogre::Vector3    w = assembly->relW();

    Ogre::Quaternion       rq( 0.0, assemblyR.x, assemblyR.y, assemblyR.z );
    rq = assQ * rq * assQ.Inverse();
    Ogre::Vector3    r( rq.x, rq.y, rq.z );

    // v = assV + W x R
    Ogre::Vector3 v = assembly->relV() + w.crossProduct( r );

    // R = assR + R
    r = assembly->relR() + r;

    assembly->

    setR( r );
    setQ( q );
    setV( v );
    setW( w );
}







static void deleteSubshapes( btCollisionShape * s )
{
    btCompoundShape * cs = dynamic_cast<btCompoundShape *>( s );
    if ( !cs )
        return;
    const int qty = cs->getNumChildShapes();
    for ( int i=0; i<qty; i++ )
    {
        btCollisionShape * ss = cs->getChildShape( i );
        delete ss;
    }
}


}




