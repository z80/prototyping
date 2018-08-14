
#include "state_intro.h"
#include "state_game.h"
#include "state_manager.h"
#include "ImguiManager.h"

using namespace Ogre;

static IntroState g_is;

template<> IntroState* Singleton<IntroState>::msSingleton = 0;

IntroState::IntroState()
    : State(),
      Ogre::Singleton<IntroState>()
{
    mRoot     = 0;
    mSceneMgr = 0;
    mViewport = 0;
    mCamera   = 0;
    mExitGame = false;
}

IntroState::~IntroState()
{

}

void IntroState::enter()
{
    mRoot = Root::getSingletonPtr();
    mSceneMgr = StateManager::getSingletonPtr()->getSceneManager();
    mCamera = mSceneMgr->createCamera( "IntroCamera" );
    mCamera->setNearClipDistance( 0.0001 );
    mViewport = StateManager::getSingletonPtr()->getRenderWindow()->addViewport( mCamera );
    mViewport->setBackgroundColour(ColourValue(0.5, 0.5, 0.5));
    mExitGame = false;

    StateManager::getSingletonPtr()->setMouseVisible( true );
}

void IntroState::exit()
{
    mSceneMgr->clearScene();
    mSceneMgr->destroyAllCameras();
    StateManager::getSingletonPtr()->getRenderWindow()->removeAllViewports();
}

void IntroState::pause()
{

}

void IntroState::resume()
{

}

bool IntroState::frameStarted(const Ogre::FrameEvent& evt)
{
    ImGui::ShowTestWindow();

    bool doExit, toGame;
    startMenuRoot( doExit, toGame );
    if ( doExit )
        return false;
    if ( toGame )
        StateManager::getSingletonPtr()->pushState( GameState::getSingletonPtr() );

    return true;
}

bool IntroState::frameEnded(const Ogre::FrameEvent& evt)
{
    if ( mExitGame )
        return false;
    return true;
}

bool IntroState::keyPressed(const OgreBites::KeyboardEvent& evt)
{
    if ( evt.keysym.sym == -1 )
    {
        //changeState( PlayState::getSingletonPtr() );
    }
    if ( evt.keysym.sym == 27 )
    {
        mExitGame = true;
    }
}

bool IntroState::keyReleased(const OgreBites::KeyboardEvent& evt)
{
    return true;
}

bool IntroState::touchMoved(const OgreBites::TouchFingerEvent& evt)
{
    return true;
}

bool IntroState::touchPressed(const OgreBites::TouchFingerEvent& evt)
{
    return true;
}

bool IntroState::touchReleased(const OgreBites::TouchFingerEvent& evt)
{
    return true;
}

bool IntroState::mouseMoved(const OgreBites::MouseMotionEvent& evt)
{
    return true;
}

bool IntroState::mouseWheelRolled(const OgreBites::MouseWheelEvent& evt)
{
    return true;
}

bool IntroState::mousePressed(const OgreBites::MouseButtonEvent& evt)
{
    return true;
}

bool IntroState::mouseReleased(const OgreBites::MouseButtonEvent& evt)
{
    return true;
}

void IntroState::startMenuRoot( bool & doExit, bool & toGame )
{
    const ImVec2 wndSz( 340, 120 );
    ImGui::SetNextWindowBgAlpha( 0.3f ); // Transparent background
    ImGui::SetNextWindowSizeConstraints( wndSz, wndSz );
    const ImVec2 windowPos = ImVec2( ImGui::GetIO().DisplaySize.x/2 - wndSz.x/2,
                                     ImGui::GetIO().DisplaySize.y/2 - wndSz.y/2 );
    const ImVec2 windowPosPivot = ImVec2( 0, 0 );

    ImGui::SetNextWindowPos( windowPos, ImGuiCond_Always, windowPosPivot );
    if ( ImGui::Begin( "Choose", 0,
                        ImGuiWindowFlags_NoMove |
                        ImGuiWindowFlags_NoTitleBar |
                        ImGuiWindowFlags_NoResize |
                        ImGuiWindowFlags_AlwaysAutoResize |
                        ImGuiWindowFlags_NoSavedSettings |
                        ImGuiWindowFlags_NoFocusOnAppearing |
                        ImGuiWindowFlags_NoNav )
       )
    {
        ImGui::Text( "Root menu" );
        toGame = ImGui::Button( "Start/continue game", ImVec2( 320, 30 ) );
        doExit = ImGui::Button( "Exit to OS",          ImVec2( 320, 30 ) );
    }
    ImGui::End();
}






