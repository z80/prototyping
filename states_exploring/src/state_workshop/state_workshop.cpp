
#include "state_workshop.h"
#include "ImguiManager.h"
#include "state_manager.h"
#include "state_intro.h"
#include "camera_ctrl.h"
#include "design_construction.h"

#include "config_reader.h"
#include "lua.hpp"
#include "lua_utils.h"

using namespace Ogre;

static WorkshopState g_is;

template<> WorkshopState* Singleton<WorkshopState>::msSingleton = 0;

WorkshopState::WorkshopState()
    : State(),
      Ogre::Singleton<WorkshopState>()
{
    paused     = false;
    designCtrl = 0;
}

WorkshopState::~WorkshopState()
{

}

const char * WorkshopState::stateName()
{
    static const char name[] = "workshop";
    return name;
}

void WorkshopState::enter()
{
    if ( !paused )
    {
        designCtrl = new Entity::DesignConstruction();
    }

    Ogre::Viewport * v = StateManager::getSingletonPtr()->getCamera()->getViewport();
    v->setBackgroundColour(ColourValue(0.7, 0.7, 0.4));
    StateManager::getSingletonPtr()->setMouseVisible( true );

    designCtrl->enter();
    resume();
}

void WorkshopState::exit()
{
    pause();

    delete designCtrl;
}

void WorkshopState::pause()
{
    paused = true;

    designCtrl->leave();
}

void WorkshopState::resume()
{
    paused = false;
    //mSceneMgr->setSkyBox( true, "Examples/CloudyNoonSkyBox" );
    Ogre::SceneManager * smgr = StateManager::getSingletonPtr()->getCamera()->getSceneManager();
    smgr->setAmbientLight( Ogre::ColourValue( 0.5, 0.5, 0.5 ) );
}

bool WorkshopState::frameStarted(const Ogre::FrameEvent& evt)
{
    if ( paused )
        return true;

    if ( designCtrl )
    {
        designCtrl->frameStarted( evt );
    }

    //ImGui::ShowTestWindow();

    //StateManager::getSingletonPtr()->scriptFrameStarted( evt );

    return true;
}

bool WorkshopState::frameEnded(const Ogre::FrameEvent& evt)
{
    if ( designCtrl )
    {
        designCtrl->frameEnded( evt );
    }
    StateManager::getSingletonPtr()->getCameraCtrl()->frameRendered( evt );
    return true;
}

bool WorkshopState::keyPressed(const OgreBites::KeyboardEvent& evt)
{
    if ( designCtrl )
    {
        if ( designCtrl->keyPressed( evt ) )
            return true;
    }
    StateManager::getSingletonPtr()->getCameraCtrl()->keyPressed( evt );
    if ( evt.keysym.sym == 27 )
        StateManager::getSingletonPtr()->pushState( IntroState::getSingletonPtr() );
    return true;
}

bool WorkshopState::keyReleased(const OgreBites::KeyboardEvent& evt)
{
    if ( designCtrl )
    {
        if ( designCtrl->keyReleased( evt ) )
            return true;
    }
    StateManager::getSingletonPtr()->getCameraCtrl()->keyReleased( evt );
    return true;
}

bool WorkshopState::touchMoved(const OgreBites::TouchFingerEvent& evt)
{
    if ( designCtrl )
    {
        if ( designCtrl->touchMoved( evt ) )
            return true;
    }
    return true;
}

bool WorkshopState::touchPressed(const OgreBites::TouchFingerEvent& evt)
{
    if ( designCtrl )
    {
        if ( designCtrl->touchPressed( evt ) )
            return true;
    }
    return true;
}

bool WorkshopState::touchReleased(const OgreBites::TouchFingerEvent& evt)
{
    if ( designCtrl )
    {
        if ( designCtrl->touchReleased( evt ) )
            return true;
    }
    return true;
}

bool WorkshopState::mouseMoved(const OgreBites::MouseMotionEvent& evt)
{
    if ( designCtrl )
    {
        if ( designCtrl->mouseMoved( evt ) )
            return true;
    }
    StateManager::getSingletonPtr()->getCameraCtrl()->mouseMoved( evt );
    return true;
}

bool WorkshopState::mouseWheelRolled(const OgreBites::MouseWheelEvent& evt)
{
    if ( designCtrl )
    {
        if ( designCtrl->mouseWheelRolled( evt ) )
            return true;
    }
    StateManager::getSingletonPtr()->getCameraCtrl()->mouseWheelRolled( evt );
    return true;
}

bool WorkshopState::mousePressed(const OgreBites::MouseButtonEvent& evt)
{
    if ( designCtrl )
    {
        if ( designCtrl->mousePressed( evt ) )
            return true;
    }
    StateManager::getSingletonPtr()->getCameraCtrl()->mousePressed( evt );
    return true;
}

bool WorkshopState::mouseReleased(const OgreBites::MouseButtonEvent& evt)
{
    if ( designCtrl )
    {
        if ( designCtrl->mouseReleased( evt ) )
            return true;
    }
    StateManager::getSingletonPtr()->getCameraCtrl()->mouseReleased( evt );
    return true;
}










