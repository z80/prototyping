
#ifndef __STATE_H_
#define __STATE_H_

#include "Ogre.h"
#include "OgreInput.h"

class State
{
public:
    State();
    virtual ~State();

    virtual void enter();
    virtual void exit();
    virtual void pause();
    virtual void resume();

    virtual bool frameStarted(const Ogre::FrameEvent& evt);
    virtual bool frameEnded(const Ogre::FrameEvent& evt);
    virtual bool keyPressed(const OgreBites::KeyboardEvent& evt);
    virtual bool keyReleased(const OgreBites::KeyboardEvent& evt);
    virtual bool touchMoved(const OgreBites::TouchFingerEvent& evt);
    virtual bool touchPressed(const OgreBites::TouchFingerEvent& evt);
    virtual bool touchReleased(const OgreBites::TouchFingerEvent& evt);
    virtual bool mouseMoved(const OgreBites::MouseMotionEvent& evt);
    virtual bool mouseWheelRolled(const OgreBites::MouseWheelEvent& evt);
    virtual bool mousePressed(const OgreBites::MouseButtonEvent& evt);
    virtual bool mouseReleased(const OgreBites::MouseButtonEvent& evt);

    void changeState( State * state );
    void pushState( State * state );
    void popState();
};



#endif


