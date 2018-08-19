
#include "state_manager.h"
#include "state.h"
#include "Ogre.h"
#include "OgreRoot.h"
#include "ImguiManager.h"

using namespace Ogre;

static StateManager g_sm;

template<> StateManager* Singleton<StateManager>::msSingleton = 0;

StateManager::StateManager()
    : OgreBites::ApplicationContext(),
      OgreBites::InputListener(),
      Ogre::Singleton<StateManager>()
{
    soundPlugin = 0;
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
        states.back()->exit();
        states.pop_back();
    }
    // store and init the new state
    states.push_back( state );
    states.back()->enter();
}

void StateManager::pushState( State * state )
{
    // pause current state
    if ( !states.empty() )
    {
        states.back()->pause();
    }
    // store and init the new state
    states.push_back( state );
    states.back()->enter();
}

void StateManager::popState()
{
    // cleanup the current state
    if ( !states.empty() )
    {
        states.back()->exit();
        states.pop_back();
    }
    // resume previous state
    if ( !states.empty() )
    {
        states.back()->resume();
    }
}

Ogre::SceneManager * StateManager::getSceneManager()
{
    return scnMgr;
}

void StateManager::setMouseVisible( bool en )
{
    setWindowGrab( !en );
}

void StateManager::shutdown()
{
    while ( !states.empty() )
    {
        states.back()->exit();
        states.pop_back();
    }

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

    initSound();
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






