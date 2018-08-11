
#include "state_intro.h"
#include "state_manager.h"

using namespace Ogre;

template<> IntroState* Singleton<IntroState>::msSingleton = 0;

IntroState::IntroState()
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
    mSceneMgr = mRoot->getSceneManager( BLANKSTRING );
    mCamera = mSceneMgr->createCamera( "IntroCamera" );
    mViewport = mRoot->getAutoCreatedWindow()->addViewport( mCamera );
    mViewport->setBackgroundColour(ColourValue(1.0, 0.0, 0.0));
    mExitGame = false;

    StateManager::getSingletonPtr()->setMouseVisible( true );
}

void IntroState::exit()
{
    mSceneMgr->clearScene();
    mSceneMgr->destroyAllCameras();
    mRoot->getAutoCreatedWindow()->removeAllViewports();
}

void IntroState::pause()
{

}

void IntroState::resume()
{

}

bool IntroState::frameStarted(const Ogre::FrameEvent& evt)
{

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






