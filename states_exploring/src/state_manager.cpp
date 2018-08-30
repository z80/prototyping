
#include "state_manager.h"
#include "state.h"
#include "Ogre.h"
#include "OgreRoot.h"
#include "ImguiManager.h"
#include "config_reader.h"
#include "lua_collision_shapes.h"
#include "lua_entity_part.h"
#include "lua_entity_world.h"
#include "lua_constraints.h"
#include "lua.hpp"
#include "lua_utils.h"

using namespace Ogre;

static StateManager g_sm;

template<> StateManager* Singleton<StateManager>::msSingleton = 0;
int luaopen_sound( lua_State * L );

StateManager::StateManager()
    : OgreBites::ApplicationContext(),
      OgreBites::InputListener(),
      Ogre::Singleton<StateManager>()
{
    soundPlugin = 0;
    initScript();
}

StateManager::~StateManager()
{

}

void StateManager::start( State * state )
{
    // Setup all the resources.
    //setup();

    // Add the first state.
    changeState( state );

    // Start rendering.
    Root * root = getRoot();
    root->startRendering();
}

void StateManager::changeState( State * state )
{
    // cleanup the current state
    if ( !states.empty() )
    {
        stateLeft( states.back()->stateName() );
        states.back()->exit();
        states.pop_back();
    }
    // store and init the new state
    states.push_back( state );
    states.back()->enter();
    stateEntered( state->stateName() );
}

void StateManager::pushState( State * state )
{
    // pause current state
    if ( !states.empty() )
    {
        states.back()->pause();
        statePaused( states.back()->stateName() );
    }
    // store and init the new state
    states.push_back( state );
    states.back()->enter();
    stateEntered( states.back()->stateName() );
}

void StateManager::popState()
{
    // cleanup the current state
    if ( !states.empty() )
    {
        stateLeft( states.back()->stateName() );
        states.back()->exit();
        states.pop_back();
    }
    // resume previous state
    if ( !states.empty() )
    {
        states.back()->resume();
        stateResumed( states.back()->stateName() );
    }
}

Ogre::SceneManager * StateManager::getSceneManager()
{
    return scnMgr;
}

Ogre::Camera * StateManager::getCamera()
{
    return mCamera;
}

Entity::EntityWorld * StateManager::getWorld()
{
    return mWorld;
}

void StateManager::setMouseVisible( bool en )
{
    setWindowGrab( !en );
}

Config::ConfigReader * StateManager::getConfigReader()
{
    return confReader;
}

void StateManager::shutdown()
{
    // First let script to finalize everything it wants in
    // the right way.
    finitScript();

    // Clear states stack.
    while ( !states.empty() )
    {
        states.back()->exit();
        states.pop_back();
    }

    Entity::EntityWorld::deleteWorld();
    mWorld = 0;

    destroyRTShaderSystem();
    finitSound();

    scnMgr->destroyAllCameras();
    scnMgr->destroyAllEntities();
    scnMgr->destroyAllMovableObjects();
    scnMgr->destroyAllManualObjects();
}

void StateManager::setup()
{
    OgreBites::ApplicationContext::setup();
    addInputListener(this);

    Ogre::ImguiManager::createSingleton();
    addInputListener(Ogre::ImguiManager::getSingletonPtr());

    // get a pointer to the already created root
    Ogre::Root* root = getRoot();
    ResourceGroupManager::getSingletonPtr()->initialiseAllResourceGroups();
    scnMgr = root->createSceneManager();
    Ogre::ImguiManager::getSingleton().init(scnMgr);

    // register our scene with the RTSS
    Ogre::RTShader::ShaderGenerator* shadergen = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
    shadergen->addSceneManager(scnMgr);

    // Debugging object to see at least something.
    /*{
        Ogre::Entity* ogreEntity = scnMgr->createEntity("ogrehead.mesh");
        Ogre::SceneNode* ogreNode = scnMgr->getRootSceneNode()->createChildSceneNode();
        ogreNode->attachObject(ogreEntity);
        ogreNode->setPosition( 0.0, 0.0, -10.0 );
        ogreNode->setScale( 0.1, 0.1, 0.1 );
        ogreNode->setVisible( true );
    }*/

    mCamera = scnMgr->createCamera( "Camera" );
    Ogre::SceneNode * cameraNode = scnMgr->getRootSceneNode()->createChildSceneNode( "CameraNode" );
    cameraNode->attachObject( mCamera );

    initSound();

    mWorld = Entity::EntityWorld::createWorld();
}

void StateManager::initSound()
{
    soundPlugin = OGRE_NEW_T( OgreOggSound::OgreOggSoundPlugin, Ogre::MEMCATEGORY_GENERAL)();

    // Register
    Root::getSingleton().installPlugin( soundPlugin );

    OgreOggSound::OgreOggSoundManager * m = OgreOggSound::OgreOggSoundManager::getSingletonPtr();
    const bool res = m->init();
    m->setSceneManager( scnMgr );
}

void StateManager::finitSound()
{
    Root::getSingleton().uninstallPlugin( soundPlugin );

    OGRE_DELETE_T( soundPlugin, OgreOggSound::OgreOggSoundPlugin, Ogre::MEMCATEGORY_GENERAL);
    soundPlugin = 0;
}

void StateManager::initScript()
{
    confReader = new Config::ConfigReader( true );
    if ( !confReader->openFile( "./main.lua" ) )
    {
        const char * err;
        confReader->error( &err );
        Ogre::LogManager::getSingletonPtr()->logMessage( err );
    }
    lua_State * L = confReader->luaState();
    luaopen_sound( L );
    luaopen_btShapes( L );
    luaopen_entityPart( L );
    luaopen_entityWorld( L );
}

void StateManager::finitScript()
{
    // Perform full collect garbage in order to destroy all the
    // objets dynamically created in the script.
    lua_State * L = confReader->luaState();

    const int top = lua_gettop( L );

    lua_pushstring( L, "finit" );
    lua_gettable( L, LUA_GLOBALSINDEX );
    const int finitRes = lua_pcall( L, 0, 0, 0 );
    if ( finitRes != 0 )
        reportError( L );

    lua_settop( L, top );

    lua_pushstring( L, "collectgarbage" );
    lua_gettable( L, LUA_GLOBALSINDEX );
    const int gcRes = lua_pcall( L, 0, 0, 0 );
    if ( gcRes != 0 )
        reportError( L );

    lua_settop( L, top );

    // Delete config reader.
    delete confReader;
}

bool StateManager::frameStarted(const Ogre::FrameEvent& evt)
{
    OgreBites::ApplicationContext::frameStarted( evt );

    Ogre::ImguiManager::getSingleton().newFrame(
        evt.timeSinceLastFrame,
        Ogre::Rect(0, 0, getRenderWindow()->getWidth(), getRenderWindow()->getHeight()));

    const bool res = states.back()->frameStarted( evt );
    return res;
}

bool StateManager::frameEnded(const Ogre::FrameEvent& evt)
{
    const bool res = states.back()->frameEnded( evt );
    return res;
}

bool StateManager::keyPressed(const OgreBites::KeyboardEvent& evt)
{
    const bool res = states.back()->keyPressed( evt );
    return res;
}

bool StateManager::keyReleased(const OgreBites::KeyboardEvent& evt)
{
    const bool res = states.back()->keyReleased( evt );
    return res;
}

bool StateManager::touchMoved(const OgreBites::TouchFingerEvent& evt)
{
    const bool res = states.back()->touchMoved( evt );
    return res;
}

bool StateManager::touchPressed(const OgreBites::TouchFingerEvent& evt)
{
    const bool res = states.back()->touchPressed( evt );
    return res;
}

bool StateManager::touchReleased(const OgreBites::TouchFingerEvent& evt)
{
    const bool res = states.back()->touchReleased( evt );
    return res;
}

bool StateManager::mouseMoved(const OgreBites::MouseMotionEvent& evt)
{
    const bool res = states.back()->mouseMoved( evt );
    return res;
}

bool StateManager::mouseWheelRolled(const OgreBites::MouseWheelEvent& evt)
{
    const bool res = states.back()->mouseWheelRolled( evt );
    return res;
}

bool StateManager::mousePressed(const OgreBites::MouseButtonEvent& evt)
{
    const bool res = states.back()->mousePressed( evt );
    return res;
}

bool StateManager::mouseReleased(const OgreBites::MouseButtonEvent& evt)
{
    const bool res = states.back()->mouseReleased( evt );
    return res;
}







