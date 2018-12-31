
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

namespace Entity
{
    class Entity;
    class EntityPart;
    class EntityPlanet;
    class EntityWorld;
    class PartManagerBase;
    class PlanetManagerBase;
    class LaunchSiteManagerBase;
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

    Ogre::SceneManager  * getSceneManager();
    Ogre::Camera        * getCamera();
    Entity::EntityWorld * getWorld();
    Entity::PartManagerBase   * getPartsManager();
    Entity::PlanetManagerBase * getPlanetsManager();
    Entity::LaunchSiteManagerBase * getSiteManager();

    void setMouseVisible( bool en );

    Config::ConfigReader * getConfigReader();
    CameraCtrl   * getCameraCtrl();
    bool rayQuery( const Ogre::Ray & ray, Entity::Entity * e, Ogre::uint32 mask = 0xFFFFFFFF );
    bool mouseQuery( Entity::Entity * e, Ogre::uint32 mask = 0xFFFFFFFF );
    bool mouseRay( Ogre::Ray & ray );

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


    Entity::EntityWorld           * mWorld;
    Entity::PartManagerBase       * mPartsManager;
    Entity::PlanetManagerBase     * mPlanetsManager;
    Entity::LaunchSiteManagerBase * mSiteManager;

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


