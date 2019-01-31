
#ifndef __STATE_MANAGER_H_
#define __STATE_MANAGER_H_

#include <OgreApplicationContext.h>
#include "OgreInput.h"
#include "OgreRTShaderSystem.h"
#include "OgreOggSoundPlugin.h"

#include "camera_ctrl.h"
#include "player_local.h"

class State;
namespace Config
{
    class ConfigReader;
}

namespace Osp
{
    class Entity;
    class Block;
    class EntityPlanet;
    class DynamicsWorld;
    class PartManagerBase;
    class PlanetManagerBase;
    class SiteManagerBase;
    class TechTree;
}

class StateManager: public OgreBites::ApplicationContext,
                    public OgreBites::InputListener,
                    public Ogre::Singleton<StateManager>
{
public:
    StateManager();
    ~StateManager();
    void start( State * state );
    void changeState( State * state );
    void pushState( State * state );
    void popState();

    // States may call this in the case it is needed or not
    // in order to save resources.
    void scriptFrameStarted( const Ogre::FrameEvent & evt );
    // Arbitrary callbacks by name.
    bool pushFunc( const char * funcName );
    bool pushString( const char * stri );
    bool pushInt( int val );
    bool pushBool( bool val );
    bool callFunc();

    Ogre::SceneManager        * getSceneManager();
    Ogre::Camera              * getCamera();
    Osp::DynamicsWorld       * getWorld();
    Osp::PartManagerBase   * getPartsManager();
    Osp::PlanetManagerBase * getPlanetsManager();
    Osp::SiteManagerBase   * getSiteManager();
    Osp::TechTree          * getTechTree();

    void setMouseVisible( bool en );

    Config::ConfigReader * getConfigReader();
    CameraCtrl   * getCameraCtrl();
    bool rayQuery(const Ogre::Ray & ray, Osp::Entity * & e, Ogre::uint32 mask = 0xFFFFFFFF );
    bool mouseQuery( Osp::Entity * & e, Ogre::uint32 mask = 0xFFFFFFFF );
    bool mouseRay( Ogre::Ray & ray );
    void mouseScreenPos( int & x, int & y );
    Ogre::Vector2 relMouseRay();

protected:
    void shutdown();
    void setup();
    void initSound();
    void finitSound();
    void initScript();
    void finitScript();

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
private:
    std::list< State * > states;
    Ogre::SceneManager  * scnMgr;
    Ogre::Camera        * mCamera;


    Osp::DynamicsWorld           * mWorld;
    Osp::PartManagerBase       * mPartsManager;
    Osp::PlanetManagerBase     * mPlanetsManager;
    Osp::SiteManagerBase * mSiteManager;
    Osp::TechTree              * mTechTree;

    OgreOggSound::OgreOggSoundPlugin * soundPlugin;
    Config::ConfigReader * confReader;
    CameraCtrl             cameraCtrl;
    Ogre::RaySceneQuery  * raySceneQuery;
    int                  mouseAtX, mouseAtY;
    bool                 windowGrab;
    int scriptArgsQty;
public:
    /// Callbacks to call by states in order to notify script about
    /// state transitions.
    void stateEntered( const std::string & name );
    void stateLeft( const std::string & name );
    void statePaused( const std::string & name );
    void stateResumed( const std::string & name );
};


#endif


