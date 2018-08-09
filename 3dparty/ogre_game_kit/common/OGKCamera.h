//
//  OGKCamera.h
//  OgreGameKit
//
//  Created by Alex on 6/19/14.
//
//

#ifndef __OgreGameKit__OGKCamera__
#define __OgreGameKit__OGKCamera__

#include <Ogre.h>
#include "OGKStaticPluginLoader.h"

// input
//#include <OISEvents.h>
//#include <OISInputManager.h>
//#include <OISKeyboard.h>
//#include <OISMouse.h>
#include "Ogre.h"
#include "OgreOverlaySystem.h"
#include "OgreApplicationContext.h"
#include "OgreInput.h"
#include "OgreRTShaderSystem.h"
#include "OgreCameraMan.h"
#include "OgreTrays.h"
#include "OgreAdvancedRenderControls.h"

#ifdef OGRE_IS_IOS
#include <OISMultiTouch.h>
class OGKCamera : public OIS::KeyListener, OIS::MultiTouchListener
#else
class OGKCamera //: public OIS::KeyListener, OIS::MouseListener
        : OgreBites::InputListener
#endif
{
public:
    OGKCamera(Ogre::SceneManager *sceneManager, Ogre::RenderWindow *renderWindow);
    ~OGKCamera();
    
    enum CameraMode {
        FREE,
        FIXED,
        FIRST_PERSON,
        THIRD_PERSON,
        THIRD_PERSON_INDIRECT
    };
    
    Ogre::Camera *getCamera();
    
    void loadFromConfig();
    
    void setEnabled(bool enabled);
    
    OGKCamera::CameraMode getMode();
    void setMode(OGKCamera::CameraMode mode);
    
    Ogre::Real getMoveSpeed();
    void setMoveSpeed(Ogre::Real moveSpeed);
    
    Ogre::Vector3 getPosition();
    void setPosition(Ogre::Vector3 position);
    
    // tightness (3rd person only)
    Ogre::Real getTightness();
    void setTightness(Ogre::Real tightness);
    
    void setTarget(Ogre::SceneNode *target);
    
    // offset (3rd person only)
    void setTargetOffset(Ogre::Vector3 offset);
    
    void update(Ogre::Real elapsedTime);
    
    bool keyPressed(const OgreBites::KeyboardEvent &keyEventRef);
    bool keyReleased(const OgreBites::KeyboardEvent &keyEventRef);
    
    bool mouseMoved(const OgreBites::MouseMotionEvent & evt );
    bool mousePressed(const OgreBites::MouseButtonEvent & evt );
    bool mouseReleased(const OgreBites::MouseButtonEvent & evt );

private:
    Ogre::Camera *mCamera;
    
    // mode
    CameraMode mMode;
    
    // nodes
    Ogre::SceneNode *mCameraNode;
    Ogre::SceneNode *mTargetNode;
    
    Ogre::Real mMoveSpeed;

    Ogre::Vector3 mTargetOffset;
    Ogre::Vector3 mLookAtOffset;
    
    // tightness (3rd person only)
    Ogre::Real mTightness;
    
    // edge buffer (3rd person only)
    Ogre::Real mEdgeBuffer;
    
    Ogre::String mCameraName;
    
    Ogre::SceneManager *mSceneManager;
    
    int mViewportOrientation;
};

#endif /* defined(__OgreGameKit__OGKCamera__) */
