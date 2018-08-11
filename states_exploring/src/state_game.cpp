
#include "state_game.h"
#include "state_manager.h"
#include "ImguiManager.h"

using namespace Ogre;

static GameState g_is;

template<> GameState* Singleton<GameState>::msSingleton = 0;

GameState::GameState()
    : State(),
      Ogre::Singleton<GameState>()
{
    mRoot     = 0;
    mSceneMgr = 0;
    mViewport = 0;
    mCamera   = 0;
    mExitState = false;
    paused     = false;
}

GameState::~GameState()
{

}

void GameState::enter()
{
    if ( !paused )
    {
        mRoot = Root::getSingletonPtr();
        mSceneMgr = StateManager::getSingletonPtr()->getSceneManager();
        mCamera = mSceneMgr->getCamera( "IntroCamera" );
        mViewport = StateManager::getSingletonPtr()->getRenderWindow()->getViewport( 0 );
    }

    mViewport->setBackgroundColour(ColourValue(0.7, 0.7, 0.4));
    StateManager::getSingletonPtr()->setMouseVisible( true );
    mExitState = false;
}

void GameState::exit()
{
    pause();
}

void GameState::pause()
{
    paused = true;
}

void GameState::resume()
{
    paused = false;
}

bool GameState::frameStarted(const Ogre::FrameEvent& evt)
{
    ImGui::ShowTestWindow();

    return true;
}

bool GameState::frameEnded(const Ogre::FrameEvent& evt)
{
    return true;
}

bool GameState::keyPressed(const OgreBites::KeyboardEvent& evt)
{
    if ( evt.keysym.sym == 27 )
    {
        StateManager::getSingletonPtr()->popState();
    }
}

bool GameState::keyReleased(const OgreBites::KeyboardEvent& evt)
{
    return true;
}

bool GameState::touchMoved(const OgreBites::TouchFingerEvent& evt)
{
    return true;
}

bool GameState::touchPressed(const OgreBites::TouchFingerEvent& evt)
{
    return true;
}

bool GameState::touchReleased(const OgreBites::TouchFingerEvent& evt)
{
    return true;
}

bool GameState::mouseMoved(const OgreBites::MouseMotionEvent& evt)
{
    return true;
}

bool GameState::mouseWheelRolled(const OgreBites::MouseWheelEvent& evt)
{
    return true;
}

bool GameState::mousePressed(const OgreBites::MouseButtonEvent& evt)
{
    return true;
}

bool GameState::mouseReleased(const OgreBites::MouseButtonEvent& evt)
{
    return true;
}






