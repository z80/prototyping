
#include "camera_ctrl.h"
#include "state_manager.h"

#include <iostream>

CameraCtrl::CameraCtrl()
{
    mEnabled = true;
    mode = Orbit;
    camera     = 0;
    nodeTarget = 0;
    targetOffset = Ogre::Vector3::ZERO;

    mOrbiting = false;
    mMoving   = false;
    mTopSpeed = 1.0;
    mGoingForward = false;
    mGoingBack    = false;
    mGoingLeft    = false;
    mGoingRight   = false;
    mGoingUp      = false;
    mGoingDown    = false;
    mFastMove     = false;

    mTranslation  = false;
    mUpVertical   = true;

    mouseSensitivity      = 0.003;
    mouseWheelSensitivity = 0.8;
    mouseTranSensisivity  = 0.01;
    orbitDist        = 10.0;
    orbitKi          = 1.595;
    orbitAt          = Ogre::Vector3( 0.0, 0.0, 10.0 );
}

CameraCtrl::~CameraCtrl()
{

}

void CameraCtrl::setEnabled( bool en )
{
    mEnabled = en;
}

void CameraCtrl::setCamera( Ogre::Camera * camera )
{
    this->camera = camera;
    Ogre::SceneNode * nodeCam = camera->getParentSceneNode();
    if ( nodeCam )
    {
        nodeCam->setInheritOrientation( true );
        nodeCam->setInheritScale( false );
    }
}

void CameraCtrl::setTargetNode( Ogre::SceneNode * nodeTarget )
{
    if ( !camera )
        return;
    Ogre::SceneNode * nodeCam = camera->getParentSceneNode();
    this->nodeTarget = nodeTarget;
    if ( nodeTarget && nodeCam )
    {
        const Ogre::Vector3    absR = nodeCam->_getDerivedPosition();
        const Ogre::Quaternion absQ = nodeCam->_getDerivedOrientation();

        const Ogre::Vector3    relR = nodeTarget->convertWorldToLocalPosition( absR );
        const Ogre::Quaternion relQ = nodeTarget->convertWorldToLocalOrientation( absQ );

        Ogre::SceneNode * camParent = nodeCam->getParentSceneNode();
        if ( camParent )
            camParent->removeChild( nodeCam );

        nodeTarget->addChild( nodeCam );

        nodeCam->setPosition( relR );
        nodeCam->setOrientation( relQ );
    }
}

void CameraCtrl::centerCamera()
{
    targetOffset = Ogre::Vector3( 0.0, 0.0, 0.0 );
}

void CameraCtrl::setMode( Mode m )
{
    mode = m;
}

void CameraCtrl::frameRendered( const Ogre::FrameEvent & evt )
{
    if ( !mEnabled )
        return;
    if ( mode == Free )
        freeMovement( evt );
    else if ( mode == Orbit )
        orbitMovement( evt );
}

bool CameraCtrl::keyPressed( const OgreBites::KeyboardEvent & evt )
{
    if ( !mEnabled )
        return false;

    OgreBites::Keycode key = evt.keysym.sym;

    // Camera keyboard controls.
    if ( mode == Orbit )
    {
        if ( key == OgreBites::SDLK_LSHIFT )
        {
            mTranslation = true;
        }
    }
    else if ( mode == Free )
    {
        if ( key == 'w' || key == OgreBites::SDLK_UP )
            mGoingForward = true;
        else if ( key == 's' || key == OgreBites::SDLK_DOWN )
            mGoingBack = true;
        else if ( key == 'a' || key == OgreBites::SDLK_LEFT )
            mGoingLeft = true;
        else if ( key == 'd' || key == OgreBites::SDLK_RIGHT )
            mGoingRight = true;
        else if ( key == OgreBites::SDLK_PAGEUP )
            mGoingUp = true;
        else if ( key == OgreBites::SDLK_PAGEDOWN )
            mGoingDown = true;
        else if ( key == OgreBites::SDLK_LSHIFT )
            mFastMove    = true;
    }

    return false;
}

bool CameraCtrl::keyReleased( const OgreBites::KeyboardEvent & evt )
{
    if ( !mEnabled )
        return false;

    const OgreBites::Keycode key = evt.keysym.sym;

    if ( mode == Orbit )
    {
        if ( key == OgreBites::SDLK_LSHIFT )
        {
            mTranslation = false;
        }
        else if ( key == 'c' )
        {
            if ( mTranslation )
            {
                centerCamera();
                return true;
            }
        }
    }

    // Looping over camera modes.
    if ( key == 'c' )
    {
        //std::cout << "Switch from " << int(mode) << std::endl;
        switch ( mode )
        {
        case Fixed:
            mode = Orbit;
            //std::cout << "    Orbit" << std::endl;
            break;
        case Orbit:
            mode = Free;
            //std::cout << "    Free" << std::endl;
            break;
        case Free:
            mode = Fixed;
            //std::cout << "    Fixed" << std::endl;
            break;
        default:
            mode = Free;
            //std::cout << "    Default Free" << std::endl;
            break;
        }
    }

    if ( mode == Free )
    {
        if ( key == 'w' || key == OgreBites::SDLK_UP )
            mGoingForward = false;
        else if ( key == 's' || key == OgreBites::SDLK_DOWN )
            mGoingBack = false;
        else if ( key == 'a' || key == OgreBites::SDLK_LEFT )
            mGoingLeft = false;
        else if ( key == 'd' || key == OgreBites::SDLK_RIGHT )
            mGoingRight = false;
        else if ( key == OgreBites::SDLK_PAGEUP )
            mGoingUp = false;
        else if ( key == OgreBites::SDLK_PAGEDOWN )
            mGoingDown = false;
        else if ( key == OgreBites::SDLK_LSHIFT )
            mFastMove    = false;
    }

    return false;
}

bool CameraCtrl::mouseMoved( const OgreBites::MouseMotionEvent & evt )
{
    if ( !mEnabled )
        return false;

    if ( mode == Orbit )
    {
        if ( !mTranslation )
        {
            orbitAdjustRotation( evt );
            return true;
        }
        else
        {
            orbitAdjustOffset( evt );
            return true;
        }
    }

    return false;
}

bool CameraCtrl::mouseWheelRolled( const OgreBites::MouseWheelEvent & evt )
{
    if ( !mEnabled )
        return false;

    if ( mode == Orbit )
    {
        orbitAdjustDistance( evt );
        return true;
    }
    /*if ( mode == Orbit && evt.y != 0 )
    {
        const Ogre::Real dist = (nodeCam->getPosition() - nodeTarget->_getDerivedPosition()).length();
        nodeCam->translate( Ogre::Vector3(0, 0, -evt.y * 0.08f * dist),
                            Ogre::Node::TS_LOCAL );
    }*/
    return false;
}

bool CameraCtrl::mousePressed( const OgreBites::MouseButtonEvent & evt )
{
    if ( !mEnabled )
        return false;

    if ( mode == Orbit )
    {
        if ( evt.button == OgreBites::BUTTON_MIDDLE )
            mOrbiting = true;
        else if ( evt.button == OgreBites::BUTTON_RIGHT )
            mMoving = true;
    }
    if ( mode != Fixed )
    {
        if ( evt.button == OgreBites::BUTTON_MIDDLE )
        {
            mOrbiting = true;
            StateManager::getSingletonPtr()->setMouseVisible( false );
        }
    }
    return false;
}

bool CameraCtrl::mouseReleased( const OgreBites::MouseButtonEvent & evt )
{
    if ( !mEnabled )
        return false;

    if ( mode == Orbit )
    {
        if ( evt.button == OgreBites::BUTTON_MIDDLE )
        {
            mOrbiting = false;
            StateManager::getSingletonPtr()->setMouseVisible( true );
            return true;
        }
        else if ( evt.button == OgreBites::BUTTON_RIGHT )
        {
            mMoving = false;
            return true;
        }
    }
    if ( mode != Fixed )
    {
        if ( evt.button == OgreBites::BUTTON_MIDDLE )
        {
            mOrbiting = false;
            StateManager::getSingletonPtr()->setMouseVisible( true );
            return true;
        }
    }

    return false;
}

std::string CameraCtrl::modeStri() const
{
    if ( mode == Free )
        return "Free";
    else if ( mode == Orbit )
        return "Orbit";
    else if ( mode == Fixed )
        return "Fixed";
    return "Weired";
}

Ogre::Real CameraCtrl::getDistToTarget() const
{
    Ogre::SceneNode * nodeCam = camera->getParentSceneNode();
    if ( !nodeCam )
        return 0.0;
    if ( !nodeTarget )
        return 0.0;
    const Ogre::Vector3 offset = nodeCam->getPosition();
    const Ogre::Real d = offset.length();
    return d;
}

void CameraCtrl::freeMovement( const Ogre::FrameEvent & evt )
{
    Ogre::SceneNode * nodeCam = camera->getParentSceneNode();
    // build our acceleration vector based on keyboard input composite
    Ogre::Vector3 accel = Ogre::Vector3::ZERO;
    Ogre::Matrix3 axes = nodeCam->getLocalAxes();
    if ( mGoingForward )
        accel -= axes.GetColumn(2);
    if ( mGoingBack )
        accel += axes.GetColumn(2);
    if ( mGoingRight )
        accel += axes.GetColumn(0);
    if ( mGoingLeft )
        accel -= axes.GetColumn(0);
    if ( mGoingUp )
        accel += axes.GetColumn(1);
    if ( mGoingDown )
        accel -= axes.GetColumn(1);

    // if accelerating, try to reach top speed in a certain time
    const Ogre::Real topSpeed = mFastMove ? mTopSpeed * 20 : mTopSpeed;
    if (accel.squaredLength() != 0)
    {
        accel.normalise();
        mVelocity += accel * topSpeed * evt.timeSinceLastFrame * 10;
    }
    // if not accelerating, try to stop in a certain time
    else
        mVelocity -= mVelocity * evt.timeSinceLastFrame * 10;

    const Ogre::Real tooSmall = std::numeric_limits<Ogre::Real>::epsilon();

    // keep camera velocity below top speed and above epsilon
    if ( mVelocity.squaredLength() > topSpeed * topSpeed )
    {
        mVelocity.normalise();
        mVelocity *= topSpeed;
    }
    else if (mVelocity.squaredLength() < tooSmall * tooSmall)
        mVelocity = Ogre::Vector3::ZERO;

    if (mVelocity != Ogre::Vector3::ZERO)
        nodeCam->translate(mVelocity * evt.timeSinceLastFrame);
}

void CameraCtrl::orbitMovement( const Ogre::FrameEvent & evt )
{
    const Ogre::Real T = 0.1;
    const Ogre::Real dt = (evt.timeSinceLastFrame < T) ? evt.timeSinceLastFrame : T;
    const Ogre::Real k = dt * orbitKi;
    const Ogre::Vector3 sp = targetOffset; //nodeTarget->getPosition();
    Ogre::Vector3 & at = orbitAt;
    const Ogre::Vector3 dr = (sp-at)*k;
    at += dr;
    // Derive current setpoint.
    Ogre::Quaternion q( 0.0, 0.0, 0.0, orbitDist );
    q = orbitQuat * q * orbitQuat.Inverse();
    const Ogre::Vector3 camAt( at.x+q.x, at.y+q.y, at.z+q.z );
    Ogre::SceneNode * nodeCam = camera->getParentSceneNode();
    nodeCam->setPosition( camAt );
    nodeCam->setOrientation( orbitQuat );

    {
        // Debugging
        Ogre::Vector3 absR = nodeCam->_getDerivedPosition();

    }
}

void CameraCtrl::orbitAdjustRotation( const OgreBites::MouseMotionEvent & evt )
{
    if ( !mOrbiting )
        return;
    const Ogre::Real dx = static_cast<Ogre::Real>( evt.xrel ) * mouseSensitivity;
    const Ogre::Real dy = static_cast<Ogre::Real>( evt.yrel ) * mouseSensitivity;
    const Ogre::Real l = std::sqrt( dx*dx + dy*dy );
    const Ogre::Real si_2 = l * 0.5;
    const Ogre::Real co_2 = 1.0;
    Ogre::Quaternion dq( co_2, -dy/l*si_2, -dx/l*si_2, 0.0 );
    orbitQuat = orbitQuat * dq;
    orbitQuat.normalise();
}

void CameraCtrl::orbitAdjustDistance( const OgreBites::MouseWheelEvent & evt )
{
    if ( evt.y > 0 )
        orbitDist *= mouseWheelSensitivity;
    else if ( evt.y < 0 )
        orbitDist /= mouseWheelSensitivity;
}

void CameraCtrl::orbitAdjustOffset( const OgreBites::MouseMotionEvent & evt )
{
    if ( !mOrbiting )
        return;
    Ogre::Vector3 right( 1.0, 0.0, 0.0 );
    Ogre::Vector3 up( 0.0, 1.0, 0.0 );
    Ogre::SceneNode * camNode = camera->getParentSceneNode();
    Ogre::Quaternion q = camNode->getOrientation();
    right = q * right;
    if ( !mUpVertical )
        up = q * up;
    const Ogre::Real dx = static_cast<Ogre::Real>( evt.xrel ) * mouseTranSensisivity;
    const Ogre::Real dy = static_cast<Ogre::Real>( evt.yrel ) * mouseTranSensisivity;
    targetOffset -= right * dx;
    targetOffset += up * dy;
}










