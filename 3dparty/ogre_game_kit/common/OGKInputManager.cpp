//
//  OGKOGKInputManager.cpp
//  OgreGameKit
//
//  Created by Alex on 6/19/14.
//
//

#include "OGKInputManager.h"
#include "OGKGame.h"

OGKInputManager *OGKInputManager::mInputManager;

OGKInputManager::OGKInputManager( void )
    : OgreBites::InputListener()
{
    mouseDx = 0.0;
    mouseDy = 0.0;
    windowSzX = 640.0;
    windowSzY = 480.0;
}

OGKInputManager::~OGKInputManager( void )
{
}

void OGKInputManager::initialise( Ogre::RenderWindow *renderWindow, bool showMouseCursor )
{
    mRenderWindow = renderWindow;
    // Get window size
    unsigned int width, height, depth;
    int left, top;
    renderWindow->getMetrics( width, height, depth, left, top );

    // Set mouse region
    this->setWindowExtents( width, height );

}

void OGKInputManager::capture( void ) {
    // Need to capture / update each device every frame
    if( mMouse ) {
        mMouse->capture();
    }
    
    if( mKeyboard ) {
        mKeyboard->capture();
    }
    
    if( mJoysticks.size() > 0 ) {
        itJoystick    = mJoysticks.begin();
        itJoystickEnd = mJoysticks.end();
        for(; itJoystick != itJoystickEnd; ++itJoystick ) {
            (*itJoystick)->capture();
        }
    }
}

void OGKInputManager::reset()
{
    initialise(mRenderWindow, false);
}

void OGKInputManager::addKeyListener( OgreBites::InputListener *keyListener, const std::string& instanceName )
{
}

void OGKInputManager::addMouseListener( OgreBites::InputListener *mouseListener, const std::string& instanceName )
{
}

void OGKInputManager::addJoystickListener( OgreBites::InputListener *joystickListener, const std::string& instanceName )
{
}

void OGKInputManager::removeKeyListener( const std::string& instanceName )
{
}

void OGKInputManager::removeMouseListener( const std::string& instanceName )
{
}

void OGKInputManager::removeJoystickListener( const std::string& instanceName )
{
}

void OGKInputManager::removeKeyListener( OgreBites::InputListener *keyListener )
{
}

void OGKInputManager::removeMouseListener( OgreBites::InputListener *mouseListener )
{
}


void OGKInputManager::removeJoystickListener( OgreBites::InputListener *joystickListener )
{
}

void OGKInputManager::removeAllListeners( void )
{
}

void OGKInputManager::removeAllKeyListeners( void )
{
}

void OGKInputManager::removeAllMouseListeners( void )
{
}

void OGKInputManager::removeAllJoystickListeners( void )
{
}

void OGKInputManager::setWindowExtents( int width, int height )
{
    windowSzX = static_cast<Ogre::Real>( width );
    windowSzY = static_cast<Ogre::Real>( height );

    Ogre::LogManager::getSingletonPtr()->getDefaultLog()->logMessage("Window Extents w: " + Ogre::StringConverter::toString(width) +
                                                                     " h: " + Ogre::StringConverter::toString(height));
}

void * OGKInputManager::getMouse( void )
{
    return 0;
}
void OGKInputManager::setMouseVisible(bool visible)
{
}
void * OGKInputManager::getKeyboard( void )
{
    return 0;
}

void * OGKInputManager::getJoystick( unsigned int index )
{
    return 0;
}

int OGKInputManager::getNumOfJoysticks( void )
{
    return 0;
}

bool OGKInputManager::keyPressed( const OgreBites::KeyboardEvent & e )
{
    /*itKeyListener    = mKeyListeners.begin();
    itKeyListenerEnd = mKeyListeners.end();
    for(; itKeyListener != itKeyListenerEnd; ++itKeyListener ) {
        if(!itKeyListener->second->keyPressed( e ))
			break;
    }*/
    
    return true;
}

bool OGKInputManager::keyReleased( const OgreBites::KeyboardEvent & e )
{
    /*itKeyListener    = mKeyListeners.begin();
    itKeyListenerEnd = mKeyListeners.end();
    for(; itKeyListener != itKeyListenerEnd; ++itKeyListener ) {
        if(!itKeyListener->second->keyReleased( e ))
			break;
    }*/
    
    return true;
}

bool OGKInputManager::mouseMoved( const OgreBites::MouseMotionEvent & e )
{
    /*itMouseListener    = mMouseListeners.begin();
    itMouseListenerEnd = mMouseListeners.end();
    for(; itMouseListener != itMouseListenerEnd; ++itMouseListener ) {
        if(!itMouseListener->second->mouseMoved( e ))
			break;
    }*/
    
    return true;
}

bool OGKInputManager::mousePressed( const OgreBites::MouseButtonEvent & e )
{
    /*itMouseListener    = mMouseListeners.begin();
    itMouseListenerEnd = mMouseListeners.end();
    for(; itMouseListener != itMouseListenerEnd; ++itMouseListener ) {
        if(!itMouseListener->second->mousePressed( e, id ))
			break;
    }*/
    
    return true;
}

bool OGKInputManager::mouseReleased( const OgreBites::MouseButtonEvent & e )
{
    /*itMouseListener    = mMouseListeners.begin();
    itMouseListenerEnd = mMouseListeners.end();
    for(; itMouseListener != mMouseListeners.end(); ++itMouseListener ) {
        if(!itMouseListener->second->mouseReleased( e, id ))
			break;
    }*/
    
    return true;
}

/*
bool OGKInputManager::povMoved( const OIS::JoyStickEvent &e, int pov )
{
    itJoystickListener    = mJoystickListeners.begin();
    itJoystickListenerEnd = mJoystickListeners.end();
    for(; itJoystickListener != itJoystickListenerEnd; ++itJoystickListener ) {
        if(!itJoystickListener->second->povMoved( e, pov ))
			break;
    }
    
    return true;
}

bool OGKInputManager::axisMoved( const OIS::JoyStickEvent &e, int axis )
{
    itJoystickListener    = mJoystickListeners.begin();
    itJoystickListenerEnd = mJoystickListeners.end();
    for(; itJoystickListener != itJoystickListenerEnd; ++itJoystickListener ) {
        if(!itJoystickListener->second->axisMoved( e, axis ))
			break;
    }
    
    return true;
}

bool OGKInputManager::sliderMoved( const OIS::JoyStickEvent &e, int sliderID )
{
    itJoystickListener    = mJoystickListeners.begin();
    itJoystickListenerEnd = mJoystickListeners.end();
    for(; itJoystickListener != itJoystickListenerEnd; ++itJoystickListener ) {
        if(!itJoystickListener->second->sliderMoved( e, sliderID ))
			break;
    }
    
    return true;
}

bool OGKInputManager::buttonPressed( const OIS::JoyStickEvent &e, int button )
{
    itJoystickListener    = mJoystickListeners.begin();
    itJoystickListenerEnd = mJoystickListeners.end();
    for(; itJoystickListener != itJoystickListenerEnd; ++itJoystickListener ) {
        if(!itJoystickListener->second->buttonPressed( e, button ))
			break;
    }
    
    return true;
}

bool OGKInputManager::buttonReleased( const OIS::JoyStickEvent &e, int button )
{
    itJoystickListener    = mJoystickListeners.begin();
    itJoystickListenerEnd = mJoystickListeners.end();
    for(; itJoystickListener != itJoystickListenerEnd; ++itJoystickListener )
    {
        if(!itJoystickListener->second->buttonReleased( e, button ))
			break;
    }
    
    return true;
}
*/

OGKInputManager * OGKInputManager::getSingletonPtr( void )
{
    if( !mInputManager ) {
        mInputManager = new OGKInputManager();
    }
    
    return mInputManager;
}


