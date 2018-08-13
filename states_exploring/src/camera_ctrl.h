
#ifndef __CAMERA_CTRL_H_
#define __CAMERA_CTRL_H_

#include "Ogre.h"
#include "OgreSingleton.h"
#include "OgreInput.h"

class CameraCtrl: public Ogre::Singleton<CameraCtrl>
{
public:
    CameraCtrl();
    ~CameraCtrl();
    void setCameraNode( Ogre::SceneNode * nodeCam );
    void setTargetNode( Ogre::SceneNode * nodeTarget );

    void frameRendered(const Ogre::FrameEvent& evt);

    /**
    Processes key presses for free-look style movement.
    */
    bool keyPressed(const OgreBites::KeyboardEvent& evt);

    /**
    Processes key releases for free-look style movement.
    */
    bool keyReleased(const OgreBites::KeyboardEvent& evt);

    /**
    Processes mouse movement differently for each style.
    */
    bool mouseMoved(const OgreBites::MouseMotionEvent& evt);

    bool mouseWheelRolled(const OgreBites::MouseWheelEvent& evt);

    /**
    Processes mouse presses. Only applies for orbit style.
    Left button is for orbiting, and right button is for zooming.
    */
    bool mousePressed(const OgreBites::MouseButtonEvent& evt);

    /**
    Processes mouse releases. Only applies for orbit style.
    Left button is for orbiting, and right button is for zooming.
    */
    bool mouseReleased(const OgreBites::MouseButtonEvent& evt);


public:
    Ogre::Real getDistToTarget() const;

    enum Mode { Free, Orbit, Fixed };

    Mode mode;
    Ogre::SceneNode * nodeCam;
    Ogre::SceneNode * nodeTarget;

    bool mOrbiting;
    bool mMoving;
    Ogre::Real    mTopSpeed;
    Ogre::Vector3 mVelocity;
    bool mGoingForward;
    bool mGoingBack;
    bool mGoingLeft;
    bool mGoingRight;
    bool mGoingUp;
    bool mGoingDown;
    bool mFastMove;

};



#endif



