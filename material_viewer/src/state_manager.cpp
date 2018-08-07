
#include "state_manager.h"


StateManager::StateManager()
{

}

StateManager::~StateManager()
{

}

void StateManager::start( State * state )
{

}

void StateManager::changeState( State * state )
{

}

void StateManager::pushState( State * state )
{

}

void StateManager::popState()
{

}

void StateManager::shutdown()
{

}

void StateManager::setup()
{

}

bool StateManager::frameStarted(const Ogre::FrameEvent& evt)
{

}

void StateManager::frameRendered(const Ogre::FrameEvent& evt)
{

}

bool StateManager::keyPressed(const OgreBites::KeyboardEvent& evt)
{
    return true;
}

bool StateManager::keyReleased(const OgreBites::KeyboardEvent& evt)
{
    return true;
}

bool StateManager::touchMoved(const OgreBites::TouchFingerEvent& evt)
{
    return true;
}

bool StateManager::touchPressed(const OgreBites::TouchFingerEvent& evt)
{
    return true;
}

bool StateManager::touchReleased(const OgreBites::TouchFingerEvent& evt)
{
    return true;
}

bool StateManager::mouseMoved(const OgreBites::MouseMotionEvent& evt)
{
    return true;
}

bool StateManager::mouseWheelRolled(const OgreBites::MouseWheelEvent& evt)
{
    return true;
}

bool StateManager::mousePressed(const OgreBites::MouseButtonEvent& evt)
{
    return true;
}

bool StateManager::mouseReleased(const OgreBites::MouseButtonEvent& evt)
{
    return true;
}


