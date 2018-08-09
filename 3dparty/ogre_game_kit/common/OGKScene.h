//
//  OGKScene.h
//  OgreGameKit
//
//  Created by Alex on 6/24/14.
//
//

#ifndef __OgreGameKit__OGKScene__
#define __OgreGameKit__OGKScene__

#include <Ogre.h>
#include "OGKStaticPluginLoader.h"
#include "Ogre.h"
#include "OgreOverlaySystem.h"
#include "OgreApplicationContext.h"
#include "OgreInput.h"
#include "OgreRTShaderSystem.h"
#include "OgreCameraMan.h"
#include "OgreTrays.h"
#include "OgreAdvancedRenderControls.h"
// input
//#include <OISKeyboard.h>
//#include <OISMouse.h>

class OGKCamera;
class OgreRayCollision;

// GUI
#include "Gui3D.h"
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
class OGKScene : public OIS::KeyListener, public OIS::MultiTouchListener
#else
class OGKScene //: public OIS::KeyListener, public OIS::MouseListener
        : OgreBites::InputListener
#endif
{
public:
    OGKScene(const Ogre::String& name);
    ~OGKScene();
    
    virtual void init();
    
    bool isRunning();
    
    virtual void loadFromFile(const Ogre::String& name);
    virtual void loadFromFile(const Ogre::String& name,
                              const std::vector<Ogre::String>& objectsToIgnore);
    virtual void saveToFile(const Ogre::String& name);
    
    virtual void onEnter();
    virtual void onEnterTransitionDidFinish();
    
    virtual void update(Ogre::Real elapsedTime);

    virtual void onExit();
    virtual void onExitTransitionDidStart();
    
    // INPUT
    bool keyPressed(const OgreBites::KeyboardEvent &keyEventRef);
    bool keyReleased(const OgreBites::KeyboardEvent &keyEventRef);
    
    bool mouseMoved(const OgreBites::MouseMotionEvent &evt);
    bool mousePressed( const OgreBites::MouseButtonEvent & evt );
    bool mouseReleased( const OgreBites::MouseButtonEvent & evt );
    
    // CAMERA
    OGKCamera *mCamera;
    
    // GUI
    Gorilla::ScreenRenderable2D *mScreen;

    Ogre::String mSceneName;

    Ogre::SceneManager *mSceneManager;
    Ogre::SceneNode *mSceneNode;
    
    Ogre::Overlay *mOverlay;
    Ogre::OverlaySystem *mOverlaySystem;
    
    virtual bool rayIntersects(const Ogre::Ray& ray, Ogre::Vector3& hitLocation, Ogre::MovableObject *hitObject, bool terrainOnly = false);
    
protected:
    bool mRunning;
    
    virtual Ogre::MovableObject *getTerrainObject();
    
    OgreRayCollision *mCollisionRay;
};

#endif /* defined(__OgreGameKit__OGKScene__) */
