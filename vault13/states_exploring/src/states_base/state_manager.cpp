
#include "state_manager.h"
#include "state.h"
#include "Ogre.h"
#include "OgreRoot.h"
#include "ImguiManager.h"

#include "config_reader.h"
#include "part_manager_base.h"
#include "planet_manager_base.h"
#include "assemblies_manager.h"
#include "entity_world.h"
#include "tech_tree.h"

#include "part_manager_test.h"
#include "site_manager_test.h"
#include "planet_manager_test.h"


#include "lua_tech_tree.h"
#include "lua_collision_shapes.h"
#include "lua_entity_part.h"
#include "lua_entity_world.h"
#include "lua_camera.h"
#include "lua_constraints.h"
#include "lua.hpp"
#include "lua_utils.h"

#include <iostream>

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
    mouseAtX = mouseAtY = 0;
    windowGrab = false;
    scriptArgsQty = 0;

    mPartsManager   = 0;
    mPlanetsManager = 0;
    mSiteManager    = 0;
    mTechTree       = 0;
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

Osp::EntityWorld * StateManager::getWorld()
{
    return mWorld;
}

Osp::PartManagerBase * StateManager::getPartsManager()
{
    return mPartsManager;
}

Osp::PlanetManagerBase * StateManager::getPlanetsManager()
{
    return mPlanetsManager;
}

Osp::SiteManagerBase * StateManager::getSiteManager()
{
    return mSiteManager;
}

Osp::TechTree * StateManager::getTechTree()
{
    return mTechTree;
}

void StateManager::setMouseVisible( bool en )
{
    windowGrab = !en;
    setWindowGrab( !en );
}

Config::ConfigReader * StateManager::getConfigReader()
{
    return confReader;
}

CameraCtrl * StateManager::getCameraCtrl()
{
    return &cameraCtrl;
}

//Ogre::RaySceneQuery   * getRaySceneQuery();
bool StateManager::rayQuery( const Ogre::Ray & ray, Osp::Entity * & ent, Ogre::uint32 mask )
{
    raySceneQuery->setRay( ray );
    raySceneQuery->setQueryMask( mask );
    Ogre::RaySceneQueryResult & res = raySceneQuery->execute();
    Ogre::RaySceneQueryResult::iterator it;
    for ( it=res.begin(); it!=res.end(); it++ )
    {
        const RaySceneQueryResultEntry & e = *it;
        if ( !e.movable )
            continue;
        const Ogre::Any & a = e.movable->getUserObjectBindings().getUserAny();
        if ( a.has_value() )
        {
            const Osp::Entity * entity = Ogre::any_cast<Osp::Entity *>( a );
            if ( entity )
            {
                ent = const_cast<Osp::Entity *>( entity );
                return true;
            }
        }
    }
    return false;
}

bool StateManager::mouseQuery( Osp::Entity * & e, Ogre::uint32 mask )
{
    Ogre::Ray ray;
    const bool rayExists = mouseRay( ray );
    if ( !rayExists )
        return false;
    const bool res = rayQuery( ray, e, mask );
    return res;
}

bool StateManager::mouseRay( Ogre::Ray & ray )
{
    /*
    const bool grab = windowGrab;
    if ( grab )
        return false;
    std::cout << "i: " << mouseAtX << " "
                       << mouseAtY << "   ";
                       */

    Ogre::RenderWindow * w = getRenderWindow();
    const Ogre::Real screenX = (Ogre::Real)((Ogre::Real)mouseAtX / (Ogre::Real)w->getWidth());
    const Ogre::Real screenY = (Ogre::Real)((Ogre::Real)mouseAtY / (Ogre::Real)w->getHeight());
    /*
    std::cout << "f: " << screenX << " "
                       << screenY << std::endl;
                       */

    const Ogre::Real Zn = 1.0;
    const Ogre::Real Zf = 2.0;

    Matrix4 mp = mCamera->getProjectionMatrix();
    const Matrix4 mv = mCamera->getViewMatrix(true);

    mp[2][2] = -(Zf+Zn)/(Zf-Zn);
    mp[2][3] = -(Zf*Zn)/(Zf-Zn)*2.0;

    const Matrix4 inverseVP = (mp * mv).inverse();

    /*
    const Matrix4 & m = inverseVP;
    std::cout << m[0][0] << " " << m[0][1] << " " << m[0][2] << " " << m[0][3] << std::endl;
    std::cout << m[1][0] << " " << m[1][1] << " " << m[1][2] << " " << m[1][3] << std::endl;
    std::cout << m[2][0] << " " << m[2][1] << " " << m[2][2] << " " << m[2][3] << std::endl;
    std::cout << m[3][0] << " " << m[3][1] << " " << m[3][2] << " " << m[3][3] << std::endl;
    */

#if OGRE_NO_VIEWPORT_ORIENTATIONMODE == 0
    // We need to convert screen point to our oriented viewport (temp solution)
    Real tX = screenX; Real a = mCamera->getOrientationMode() * Math::HALF_PI;
    screenX = Math::Cos(a) * (tX-0.5f) + Math::Sin(a) * (screenY-0.5f) + 0.5f;
    screenY = Math::Sin(a) * (tX-0.5f) + Math::Cos(a) * (screenY-0.5f) + 0.5f;
    if ((int)mCamera->getOrientationMode()&1) screenY = 1.f - screenY;
#endif

    const Real nx = (2.0f * screenX) - 1.0f;
    const Real ny = 1.0f - (2.0f * screenY);
    /*
    std::cout << "n: " << nx << " "
                       << ny << std::endl;
                       */
    Vector3 nearPoint(nx, ny, -1.f);
    /*std::cout << "np: " << nearPoint.x << " "
                        << nearPoint.y << " "
                        << nearPoint.z << std::endl;*/
    // Use midPoint rather than far point to avoid issues with infinite projection
    Vector3 midPoint (nx, ny,  0.0f);
    /*std::cout << "mp: " << midPoint.x << " "
                        << midPoint.y << " "
                        << midPoint.z << std::endl;*/

    // Get ray origin and ray target on near plane in world space
    Vector3 rayOrigin, rayTarget;

    rayOrigin = inverseVP * nearPoint;
    /*std::cout << "ro: " << rayOrigin.x << " "
                        << rayOrigin.y << " "
                        << rayOrigin.z << std::endl;*/
    rayTarget = inverseVP * midPoint;
    /*std::cout << "rt: " << rayTarget.x << " "
                        << rayTarget.y << " "
                        << rayTarget.z << std::endl;*/

    Vector3 rayDirection = rayTarget - rayOrigin;

    /*std::cout << "d: " << rayDirection.x << " "
                       << rayDirection.y << " "
                       << rayDirection.z << std::endl;
    std::cout << "o: " << rayOrigin.x << " "
                       << rayOrigin.y << " "
                       << rayOrigin.z << std::endl;*/

    rayDirection.normalise();

    ray.setOrigin( rayOrigin );
    ray.setDirection( rayDirection );
    return true;
}

void StateManager::mouseScreenPos( int & x, int & y )
{
    x = mouseAtX;
    y = mouseAtY;
}

Vector2 StateManager::relMouseRay()
{
    Ogre::Viewport * v = mCamera->getViewport();
    int left, top, width, height;
    v->getActualDimensions( left, top, width, height );
    const Ogre::Real x = static_cast<Ogre::Real>( mouseAtX - left ) /
            static_cast<Ogre::Real>( width );
    const Ogre::Real y = static_cast<Ogre::Real>( mouseAtY - top ) /
            static_cast<Ogre::Real>( height );
    return Ogre::Vector2( x, y );
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


    delete mTechTree;
    delete mPartsManager;
    delete mSiteManager;
    delete mPlanetsManager;

    scnMgr->destroyAllCameras();
    scnMgr->destroyAllEntities();
    scnMgr->destroyAllMovableObjects();
    scnMgr->destroyAllManualObjects();
    scnMgr->destroyQuery( raySceneQuery );

    Osp::EntityWorld::deleteWorld();
    mWorld = 0;

    destroyRTShaderSystem();
    finitSound();
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
    Ogre::ResourceGroupManager::getSingletonPtr()->initialiseResourceGroup( ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME );
    Ogre::ResourceGroupManager::getSingletonPtr()->loadResourceGroup( ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME );

    mCamera = scnMgr->createCamera( "Camera" );
    Ogre::SceneNode * cameraNode = scnMgr->getRootSceneNode()->createChildSceneNode( "CameraNode" );
    cameraNode->attachObject( mCamera );
    cameraCtrl.setCamera( mCamera );

    raySceneQuery = scnMgr->createRayQuery( Ogre::Ray() );

    initSound();

    mWorld          = Osp::EntityWorld::createWorld();
    mPartsManager   = new Osp::PartManagerTest();
    mSiteManager    = new Osp::SiteManagerTest();
    mPlanetsManager = new Osp::PlanetManagerTest();
    mPlanetsManager->create();
    mSiteManager->create();
    mTechTree = new Osp::TechTree();

    // After all needed objects are created init script.
    initScript();

    mTechTree->load( getConfigReader() );
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

    lua_State * L = confReader->luaState();
    luaopen_sound( L );
    luaopen_btShapes( L );
    luaopen_entityPart( L );
    luaopen_entityWorld( L );
    luaopen_camera( L );
    luaopen_techTree( L );

    if ( !confReader->openFile( "./resources/lua/main.lua" ) )
    {
        const char * err;
        confReader->error( &err );
        Ogre::LogManager * lm = Ogre::LogManager::getSingletonPtr();
        if ( lm )
            lm->logMessage( err );
    }
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
    mouseAtX = evt.x;
    mouseAtY = evt.y;
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







