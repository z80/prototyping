//
//  OGKGame.h
//  OgreGameKit
//
//  Created by Alex on 5/20/14.
//
//

#ifndef __OgreGameKit__OGKGame__
#define __OgreGameKit__OGKGame__

// gfx
#include "Ogre.h"
#include "OgreOverlaySystem.h"
#include "OgreApplicationContext.h"

#include "OGKGUIThemes.h"

// input
#include "OGKInputManager.h"

// defines
#include "OGKStaticPluginLoader.h"

// camera
#include "OGKCamera.h"

// audio
#include "OGKAudio.h"

// scene
#include "OGKScene.h"
#include "OGKSceneManager.h"

namespace Gorilla {
    class Caption;
    class Screen;
}

#define OGKLOG(m) OGKGame::getSingletonPtr()->mLog->logMessage(m)

class OGKGame //: public Ogre::Singleton<OGKGame>, OIS::KeyListener, OIS::MouseListener
        : public Ogre::Singleton<OGKGame>, OgreBites::InputListener
{
public:
	OGKGame();
	~OGKGame();
    
	bool isOgreToBeShutDown()const{return mShutdown;}
    
    Ogre::ConfigFile* getGameConfig();
    
    bool keyPressed( const OgreBites::KeyboardEvent & keyEventRef );
    bool keyReleased( const OgreBites::KeyboardEvent & keyEventRef );
    
    virtual bool renderOneFrame();
    virtual bool renderOneFrame(double timeSinceLastFrame);
    virtual void setup();
    virtual void shutdown();
    virtual void start();
    virtual void update(double timeSinceLastFrame);
    
    bool mouseMoved( const OgreBites::MouseMotionEvent & evt );
    bool mousePressed( const OgreBites::MouseButtonEvent & evt );
    bool mouseReleased( const OgreBites::MouseButtonEvent & evt );
	
	Ogre::Root*                 mRoot;
	Ogre::SceneManager*			mSceneManager;
	Ogre::RenderWindow*			mRenderWindow;
	OGKCamera*                  mCamera;
	Ogre::Log*                  mLog;
	Ogre::Timer*				mTimer;
    Ogre::OverlaySystem*        mOverlaySystem;
    
    OGKSceneManager*            mGameSceneManager;
    
    OGKDefaultGUITheme          *mDefaultGUITheme;
    
protected:
    Ogre::String                 mResourcePath;
    Ogre::ConfigFile            *mConfig;
    
private:
	OGKGame(const OGKGame&);
	OGKGame& operator= (const OGKGame&);
    
    bool _init(Ogre::String wndTitle);
    
    void _initInput();
    void _initConsole();
    void _initRenderSystem();
    void _initResources();
    void _initOverlays();
    void _loadGameConfig();
    
    // fps
    Gorilla::Screen*  mOverlayScreen;
    Gorilla::Caption* mFPS;
    
	double                      mTimeSinceLastFrame;
	double                      mStartTime;
	bool                        mShutdown;
    
#ifdef OGRE_STATIC_LIB
    Ogre::StaticPluginLoader    mStaticPluginLoader;
#endif
};

#endif
