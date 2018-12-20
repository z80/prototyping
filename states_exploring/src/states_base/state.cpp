
#include "state.h"
#include "state_manager.h"

State::State()
{

}

State::~State()
{

}

void State::enter()
{

}

void State::exit()
{

}

void State::pause()
{

}

void State::resume()
{

}

bool State::frameStarted( const Ogre::FrameEvent & evt )
{
    return true;
}

bool State::frameEnded( const Ogre::FrameEvent & evt )
{
    return true;
}

bool State::keyPressed( const OgreBites::KeyboardEvent & evt )
{
    return true;
}

bool State::keyReleased( const OgreBites::KeyboardEvent & evt )
{
    return true;
}

bool State::touchMoved( const OgreBites::TouchFingerEvent & evt )
{
    return true;
}

bool State::touchPressed( const OgreBites::TouchFingerEvent & evt )
{
    return true;
}

bool State::touchReleased( const OgreBites::TouchFingerEvent & evt )
{
    return true;
}

bool State::mouseMoved( const OgreBites::MouseMotionEvent & evt )
{
    return true;
}

bool State::mouseWheelRolled( const OgreBites::MouseWheelEvent & evt )
{
    return true;
}

bool State::mousePressed( const OgreBites::MouseButtonEvent & evt )
{
    return true;
}

bool State::mouseReleased( const OgreBites::MouseButtonEvent & evt )
{
    return true;
}

void State::changeState( State * state )
{
    StateManager::getSingletonPtr()->changeState( state );
}

void State::pushState( State * state )
{
    StateManager::getSingletonPtr()->pushState( state );
}

void State::popState()
{
    StateManager::getSingletonPtr()->popState();
}




