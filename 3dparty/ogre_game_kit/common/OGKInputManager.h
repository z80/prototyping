//
//  OGKInputManager.h
//  OgreGameKit
//
//  Created by Alex on 6/19/14.
//
//

#ifndef __OgreGameKit__OGKInputManager__
#define __OgreGameKit__OGKInputManager__

#include <Ogre.h>
#include "OGKStaticPluginLoader.h"

//#include <OISEvents.h>
//#include <OISInputManager.h>
//#include <OISKeyboard.h>
//#include <OISMouse.h>
//#include <OISJoyStick.h>
#include "Ogre.h"
#include "OgreOverlaySystem.h"
#include "OgreApplicationContext.h"
#include "OgreInput.h"
#include "OgreRTShaderSystem.h"
#include "OgreCameraMan.h"
#include "OgreTrays.h"
#include "OgreAdvancedRenderControls.h"

class OGKInputManager //: public OIS::KeyListener, public OIS::MouseListener, public OIS::JoyStickListener {
        : OgreBites::InputListener
{
public:
    virtual ~OGKInputManager( void );
    
    void initialise( Ogre::RenderWindow *renderWindow, bool showMouseCursor = false );
    void reset();
    void capture( void );
    
    void addKeyListener( OgreBites::InputListener *keyListener, const std::string & instanceName );
    void addMouseListener( OgreBites::InputListener *mouseListener, const std::string& instanceName );
    void addJoystickListener( OgreBites::InputListener *joystickListener, const std::string& instanceName );
    
    void removeAllListeners( void );

    void removeKeyListener( const std::string& instanceName );
    void removeKeyListener( OgreBites::InputListener *keyListener );
    void removeAllKeyListeners( void );

    void removeMouseListener( const std::string& instanceName );
    void removeMouseListener( OgreBites::InputListener * mouseListener );

    void removeAllMouseListeners( void );
    void removeJoystickListener( OgreBites::InputListener *joystickListener );
    void removeJoystickListener( const std::string& instanceName );
    void removeAllJoystickListeners( void );
    
    void setWindowExtents( int width, int height );
    

    void * getMouse( void );
    void setMouseVisible(bool visible);
    void * getKeyboard( void );
    void * getJoystick( unsigned int index );
    
    int getNumOfJoysticks( void );
    
    static OGKInputManager* getSingletonPtr( void );
private:
    OGKInputManager( void );
    OGKInputManager( const OGKInputManager& ) { }
    OGKInputManager & operator = ( const OGKInputManager& );
    
    bool keyPressed( const OgreBites::KeyboardEvent &e );
    bool keyReleased( const OgreBites::KeyboardEvent &e );
    
    bool mouseMoved(const OgreBites::MouseMotionEvent &evt);
    bool mousePressed(const OgreBites::MouseButtonEvent &evt );
    bool mouseReleased(const OgreBites::MouseButtonEvent &evt );
    
    static OGKInputManager * mInputManager;
        
    Ogre::RenderWindow * mRenderWindow;

// My modifications.
public:
    bool isKeyDown( OgreBites::Keycode key );
    void mouseMovement( Ogre::Real & dx, Ogre::Real & dy );
    void mousePosition( Ogre::Real & dx, Ogre::Real & dy );

    Ogre::Real mouseDx, mouseDy;
    Ogre::Real mouseX, mouseY;
    Ogre::Real windowSzX, windowSzY;
    Ogre::set< OgreBites::Keycode > pressedKeys;
};

#endif /* defined(__OgreGameKit__OGKInputManager__) */
