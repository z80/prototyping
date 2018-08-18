
#ifndef __STATE_GAME_H_
#define __STATE_GAME_H_

#include "state.h"
#include "entity.h"
#include "entity_world.h"
#include "entity_part.h"
#include "entity_planet.h"

class GameState: public State,
                  public Ogre::Singleton<GameState>
{
public:
    GameState();
    ~GameState();

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
    void debugOverlay();

    Ogre::Root * mRoot;
    Ogre::SceneManager * mSceneMgr;
    Ogre::Viewport * mViewport;
    Ogre::Camera * mCamera;
    Ogre::SceneNode * mCameraNode;
    bool mExitState;
    bool paused;
    bool disableMouseCtrl;


    Entity::EntityWorld * world;
    Entity::EntityPart  //* plane,
                        * cube;
    Entity::EntityPlanet * planet;

    void createObjects();
    void destroyObjects();
};



#endif


