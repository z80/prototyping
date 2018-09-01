
#ifndef __CAMERA_CTRL_H_
#define __CAMERA_CTRL_H_

#include "Ogre.h"
#include "OgreSingleton.h"
#include "OgreInput.h"

class CameraCtrl
{
public:
    enum Mode { Free, Orbit, Fixed };

    CameraCtrl();
    ~CameraCtrl();
    void setEnabled( bool en );
    void setCameraNode( Ogre::SceneNode * nodeCam );
    void setTargetNode( Ogre::SceneNode * nodeTarget );

    void setMode( Mode m );

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


    std::string modeStri() const;
public:
    Ogre::Real getDistToTarget() const;
    void freeMovement( const Ogre::FrameEvent & evt );
    void orbitMovement( const Ogre::FrameEvent & evt );
    void orbitAdjustRotation( const OgreBites::MouseMotionEvent & evt );
    void orbitAdjustDistance( const OgreBites::MouseWheelEvent & evt );


    bool mEnabled;
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

    Ogre::Real       mouseSensitivity,
                     mouseWheelSensitivity,
                     orbitDist,
                     orbitKi;
    Ogre::Quaternion orbitQuat;
    Ogre::Vector3    orbitAt;
};



#endif



