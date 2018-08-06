
#ifndef __STATE_MANAGER_H_
#define __STATE_MANAGER_H_

#include <OgreApplicationContext.h>
#include "OgreInput.h"
#include "OgreRTShaderSystem.h"


class State;

class StateManager: public OgreBites::ApplicationContext,
                    public OgreBites::InputListener
{
public:
    StateManager();
    ~StateManager();
    void start( State * state );
    void changeState( State * state );
    void pushState( State * state );
    void popState();
protected:
    void shutdown();
    void setup();

    virtual bool frameStarted(const Ogre::FrameEvent& evt);
    virtual void frameRendered(const Ogre::FrameEvent& evt);
    virtual bool keyPressed(const OgreBites::KeyboardEvent& evt);
    virtual bool keyReleased(const OgreBites::KeyboardEvent& evt);
    virtual bool touchMoved(const OgreBites::TouchFingerEvent& evt);
    virtual bool touchPressed(const OgreBites::TouchFingerEvent& evt);
    virtual bool touchReleased(const OgreBites::TouchFingerEvent& evt);
    virtual bool mouseMoved(const OgreBites::MouseMotionEvent& evt);
    virtual bool mouseWheelRolled(const OgreBites::MouseWheelEvent& evt);
    virtual bool mousePressed(const OgreBites::MouseButtonEvent& evt);
    virtual bool mouseReleased(const OgreBites::MouseButtonEvent& evt);
private:
    std::list< State * > states;
};


#endif


