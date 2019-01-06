
#ifndef __STATE_WORKSHOP_H_
#define __STATE_WORKSHOP_H_

#include "state.h"
#include "entity.h"
#include "group_item.h"
#include "ImguiManager.h"

namespace Entity
{
    class DesignConstruction;
}

class WorkshopState: public State,
                     public Ogre::Singleton<WorkshopState>
{
public:
    WorkshopState();
    ~WorkshopState();

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

    bool paused;
    Entity::DesignConstruction * designCtrl;
};



#endif


