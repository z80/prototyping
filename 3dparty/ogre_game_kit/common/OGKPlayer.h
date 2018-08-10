//
//  OGKPlayer.h
//  OgreGameKit
//
//  Created by Alex on 6/19/14.
//
//

#ifndef __OgreGameKit__OGKPlayer__
#define __OgreGameKit__OGKPlayer__

#include <Ogre.h>
#include "OGKStaticPluginLoader.h"
#include "OgreApplicationContext.h"

// input
//#include <OISEvents.h>
//#include <OISInputManager.h>
//#include <OISKeyboard.h>
//#include <OISMouse.h>

class OGKScene;

class OGKPlayer //: public OIS::KeyListener, OIS::MouseListener
        : public OgreBites::InputListener
{
public:
    OGKPlayer(Ogre::SceneManager *sceneManager);
    ~OGKPlayer();
    
    enum MovingState {
        NONE,
        USER_CONTROLLED,
        START_MOVING,
        MOVING,
        ARRIVED,
        AT_DESTINATION
    };
    
    void attack(Ogre::Vector3 position);
    void attack(Ogre::Entity *entity);
    
    bool getEnabled();
    Ogre::Vector3 getDestination();
    Ogre::Real getMoveSpeed();
    Ogre::Real getRotateSpeed();
    Ogre::SceneNode *getSceneNode();

    inline Ogre::Real getHealth() { return mHealth; }
    inline Ogre::Real getMaxHealth() { return mMaxHealth; }
    
    void init();
    
    inline bool isAlive() { return mHealth >= 0.001; }
    
    void setHealth(Ogre::Real amount, Ogre::Real maxAmount);
    void heal(Ogre::Real amount);
    void damage(Ogre::Real amount);
    
    bool keyPressed(const OgreBites::KeyboardEvent & keyEventRef);
    bool keyReleased(const OgreBites::KeyboardEvent & keyEventRef);
    
    bool mouseMoved(const OgreBites::MouseMotionEvent &evt);
    bool mousePressed(const OgreBites::MouseButtonEvent &evt);
    bool mouseReleased(const OgreBites::MouseButtonEvent &evt);
    
    void loadFromConfig();
    
    void update(Ogre::Real elapsedTime);
    
    void setEnabled(bool enabled);
    void setDestination(Ogre::Vector3 destination);
    void setMoveSpeed(Ogre::Real speed);
    void setRotateSpeed(Ogre::Real speed);
    
    void snapToTerrain();
    Ogre::Entity *mEntity;

private:
    Ogre::Vector3 mDestination;
    bool mEnabled;
    
    Ogre::Real mHealth;
    Ogre::Real mMaxHealth;
    
    MovingState mMovingState;
    Ogre::Real mMoveSpeed;
    Ogre::Real mRotateSpeed;
    Ogre::SceneNode *mSceneNode;
    Ogre::SceneManager *mSceneManager;
    
    //OGKInGameScene *mScene;
    OGKScene *mScene;
};

#endif /* defined(__OgreGameKit__OGKPlayer__) */
