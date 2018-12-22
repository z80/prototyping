
#include "entity_part.h"
#include "entity_planet.h"
#include "entity_world.h"
#include "state_manager.h"

namespace Entity
{

static void deleteSubshapes( btCollisionShape * s );

EntityPart::EntityPart()
    : Entity()
{
    actionGroup       = 0;
    isSeleted         = false;
    doShowContextMenu = false;

    visualEntity   = 0;
    sceneNode      = 0;
    rigidBody      = 0;
    collisionShape = 0;
    bodyState      = 0;

    parent      = 0;
    nearSurface = true;
}

EntityPart::~EntityPart()
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
}

void EntityPart::loadResources()
{

}

void EntityPart::setActionGroup( Ogre::int32 group )
{
    actionGroup = group;
}

void EntityPart::action( Ogre::int32 actionGroup, const Ogre::String & name, Ogre::Real value )
{
    // By default do nothing.
}

void EntityPart::drawGui()
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

void EntityPart::drawContextMenu()
{

}

void EntityPart::computeAirFriction()
{
    //airMesh.
}

void EntityPart::selectEvent()
{

}

void EntityPart::unselectEvent()
{

}

void EntityPart::contextMenuEvent()
{

}

void EntityPart::speed( Ogre::Vector3 & v ) const
{
    if ( !rigidBody )
    {
        v.x = 0.0;
        v.y = 0.0;
        v.z = 0.0;
        return;
    }

    btVector3 vel = rigidBody->getLinearVelocity();
    v.x = vel.x();
    v.y = vel.y();
    v.z = vel.z();
}

void EntityPart::speed( Ogre::Real & v ) const
{
    if ( !rigidBody )
    {
        v = 0.0;
        return;
    }

    btVector3 vel = rigidBody->getLinearVelocity();
    v = vel.length();
}

void EntityPart::setSpeed( Ogre::Vector3 & v )
{
    btVector3 vel( v.x, v.y, v.z );
    rigidBody->setLinearVelocity( vel );
}




bool EntityPart::setEntity( const char * mesh, const char * material )
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

bool EntityPart::setMaterial( const char * material )
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

bool EntityPart::setMask( Ogre::uint32 mask )
{
    if ( !visualEntity )
        return false;

    visualEntity->setQueryFlags( mask );
    return true;
}

bool EntityPart::setAirMesh( const std::string & meshFileName )
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

void EntityPart::addConnectionPoint( const ConnectionPoint & pt )
{
    connections.push_back( pt );
}

void EntityPart::setMass( Ogre::Real & m )
{
    mass = m;
}

void EntityPart::setInertia( const Ogre::Vector3 & i )
{
    inertia = i;
}

void EntityPart::setCollisionShape( btCollisionShape * shape )
{
    deleteCollisionShape();
    collisionShape = shape;
}

void EntityPart::initDynamics()
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

void EntityPart::applyForce( const Ogre::Vector3 & at, const Ogre::Vector3 & f )
{
    rigidBody->applyForce( btVector3(f.x, f.y, f.z), btVector3( at.x, at.y, at.z ) );
}

void EntityPart::applyTorque( const Ogre::Vector3 & p )
{
    rigidBody->applyTorque( btVector3( p.x, p.y, p.z ) );
}

void EntityPart::setPosition( const Ogre::Vector3 & at )
{
    btMotionState * st = rigidBody->getMotionState();
    btTransform t;
    st->getWorldTransform( t );
    t.setOrigin( btVector3( at.x, at.y, at.z ) );
    st->setWorldTransform( t );
}

Ogre::Vector3 EntityPart::position() const
{
    btMotionState * st = rigidBody->getMotionState();
    btTransform t;
    st->getWorldTransform( t );
    const btVector3 at = t.getOrigin();
    const Ogre::Vector3 v( at.x(), at.y(), at.z() );
    return v;
}

void EntityPart::setRotation( const Ogre::Quaternion & q )
{
    btMotionState * st = rigidBody->getMotionState();
    btTransform t;
    st->getWorldTransform( t );
    t.setRotation( btQuaternion( q.x, q.y, q.z, q.w ) );
    st->setWorldTransform( t );
}

Ogre::Quaternion EntityPart::rotation() const
{
    btMotionState * st = rigidBody->getMotionState();
    btTransform t;
    st->getWorldTransform( t );
    const btQuaternion bq = t.getRotation();
    const Ogre::Quaternion q( bq.w(), bq.x(), bq.y(), bq.z() );
    return q;
}

void EntityPart::setParent( EntityPlanet * planet, bool nearSurface )
{
    if ( parent!=planet )
    {
        if ( parent )
        {
            Ogre::Vector3 parentV = parent->absoluteV();
            if ( this->nearSurface )
            {

                //const Ogre::Vector3 surfV = parent->rotV( at );
            }
        }
    }


}

bool EntityPart::addSound( const std::string & fileName, const std::string & name )
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

void EntityPart::finalizeSounds()
{
    std::sort_heap( sounds.begin(), sounds.end(), Sound::Lt );
}

bool EntityPart::playSound( const std::string & name, bool looped )
{
    std::vector<Sound>::iterator it = std::lower_bound( sounds.begin(), sounds.end(), name, Sound::Lt );
    if ( it == sounds.end() )
        return false;
    Sound & ss = *it;
    OgreOggSound::OgreOggISound * s = ss.s;
    s->play( true );

    return true;
}

bool EntityPart::stopSound( const std::string & name )
{
    std::vector<Sound>::iterator it = std::lower_bound( sounds.begin(), sounds.end(), name, Sound::Lt );
    if ( it == sounds.end() )
        return false;
    Sound & ss = *it;
    OgreOggSound::OgreOggISound * s = ss.s;
    s->stop( true );

    return true;
}

void EntityPart::deleteRigidBody()
{
    if ( rigidBody )
    {
        EntityWorld * w = EntityWorld::getSingletonPtr();
        if ( w && w->phyWorld )
            w->phyWorld->removeRigidBody( rigidBody );
        if ( bodyState )
            delete bodyState;

        delete rigidBody;

        rigidBody = 0;
        bodyState = 0;
    }
}

void EntityPart::deleteCollisionShape()
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

void EntityPart::deleteVisual()
{
    Ogre::SceneManager * scnMgr = StateManager::getSingletonPtr()->getSceneManager();
    if ( scnMgr )
    {
        if ( visualEntity );
        {
            scnMgr->destroyEntity( visualEntity );
            visualEntity = 0;
        }

        if ( sceneNode )
        {
            scnMgr->destroySceneNode( sceneNode );
            sceneNode = 0;
        }
    }
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




