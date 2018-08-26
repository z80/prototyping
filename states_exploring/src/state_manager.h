
#ifndef __STATE_MANAGER_H_
#define __STATE_MANAGER_H_

#include <OgreApplicationContext.h>
#include "OgreInput.h"
#include "OgreRTShaderSystem.h"
#include "OgreOggSoundPlugin.h"

class State;
namespace Config
{
    class ConfigReader;
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

    Ogre::SceneManager * getSceneManager();
    void setMouseVisible( bool en );

    Config::ConfigReader * getConfigReader();

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
    Ogre::SceneManager * scnMgr;
    OgreOggSound::OgreOggSoundPlugin * soundPlugin;
    Config::ConfigReader * confReader;
public:
    /// Callbacks to call by states in order to notify script about
    /// state transitions.
    void stateEntered( const std::string & name );
    void stateLeft( const std::string & name );
    void statePaused( const std::string & name );
    void stateResumed( const std::string & name );
};


#endif


