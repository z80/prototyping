
#ifndef __STATE_INTRO_H_
#define __STATE_INTRO_H_

#include "state.h"

class IntroState: public State,
                  public Ogre::Singleton<IntroState>
{
public:
    IntroState();
    ~IntroState();

    const char * stateName();

    void enter();
    void exit();
    void pause();
    void resume();

    bool frameStarted(const Ogre::FrameEvent& evt);
    bool frameEnded(const Ogre::FrameEvent& evt);
    bool keyPressed(const OgreBites::KeyboardEvent& evt);
    bool keyReleased(const OgreBites::KeyboardEvent& evt);
    bool touchMoved(const OgreBites::TouchFingerEvent& evt);
    bool touchPressed(const OgreBites::TouchFingerEvent& evt);
    bool touchReleased(const OgreBites::TouchFingerEvent& evt);
    bool mouseMoved(const OgreBites::MouseMotionEvent& evt);
    bool mouseWheelRolled(const OgreBites::MouseWheelEvent& evt);
    bool mousePressed(const OgreBites::MouseButtonEvent& evt);
    bool mouseReleased(const OgreBites::MouseButtonEvent& evt);

private:
    void startMenuRoot( bool & doExit, bool & toGame );
    void initSound();
    void pauseSound();
    void resumeSound();

    Ogre::Root * mRoot;
    Ogre::SceneManager * mSceneMgr;
    Ogre::Viewport * mViewport;
    Ogre::Camera * mCamera;
    bool mExitGame;
    int instances;
};



#endif


