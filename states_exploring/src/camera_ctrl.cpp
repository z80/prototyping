
#include "camera_ctrl.h"
#include "state_manager.h"

#include <iostream>

static CameraCtrl g_cc;

template<> CameraCtrl * Ogre::Singleton<CameraCtrl>::msSingleton = 0;

CameraCtrl::CameraCtrl()
    : Singleton<CameraCtrl>()
{
    mode = Free;
    nodeCam    = 0;
    nodeTarget = 0;

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
}

CameraCtrl::~CameraCtrl()
{

}

void CameraCtrl::setCameraNode( Ogre::SceneNode * nodeCam )
{
    this->nodeCam = nodeCam;
    if ( !nodeCam )
        this->nodeTarget = 0;
}

void CameraCtrl::setTargetNode( Ogre::SceneNode * nodeTarget )
{
    this->nodeTarget = nodeTarget;
    if ( this->nodeCam && this->nodeTarget )
    {
        Ogre::SceneNode * parent = this->nodeCam->getParentSceneNode();
        if ( parent )
            parent->removeChild( this->nodeCam );
        this->nodeTarget->addChild( this->nodeCam );
    }
}

void CameraCtrl::frameRendered( const Ogre::FrameEvent & evt )
{
    if ( mode == Free )
    {
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
}

bool CameraCtrl::keyPressed( const OgreBites::KeyboardEvent & evt )
{
    OgreBites::Keycode key = evt.keysym.sym;

    // Camera keyboard controls.
    if ( mode == Free )
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
            mFastMove = true;
    }

    return true;
}

bool CameraCtrl::keyReleased( const OgreBites::KeyboardEvent & evt )
{
    const OgreBites::Keycode key = evt.keysym.sym;

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
            mFastMove = false;
    }

    return true;
}

bool CameraCtrl::mouseMoved( const OgreBites::MouseMotionEvent & evt )
{
    Ogre::Vector3 mOffset( 0.0, 0.0, 0.0 );

    if ( mode == Orbit )
    {
        const Ogre::Real dist = getDistToTarget();

        if ( mOrbiting )   // yaw around the target, and pitch locally
        {
            nodeCam->setPosition( nodeTarget->_getDerivedPosition() + mOffset );

            nodeCam->yaw(Ogre::Degree(-evt.xrel * 0.25f), Ogre::Node::TS_PARENT);
            nodeCam->pitch(Ogre::Degree(-evt.yrel * 0.25f));

            nodeCam->translate(Ogre::Vector3(0, 0, dist), Ogre::Node::TS_LOCAL);
            // don't let the camera go over the top or around the bottom of the target
        }
        else if ( mMoving )  // move the camera along the image plane
        {
            Ogre::Vector3 delta = nodeCam->getOrientation() * Ogre::Vector3(-evt.xrel, evt.yrel, 0);
            // the further the camera is, the faster it moves
            delta *= dist / 1000.0f;
            mOffset += delta;
            nodeCam->translate( delta );
        }
    }
    else if ( mode == Free )
    {
        if ( mOrbiting )
        {
            nodeCam->yaw( Ogre::Degree( -evt.xrel * 0.15f ), Ogre::Node::TS_PARENT );
            nodeCam->pitch( Ogre::Degree( -evt.yrel * 0.15f ) );
        }
    }


    return true;
}

bool CameraCtrl::mouseWheelRolled( const OgreBites::MouseWheelEvent & evt )
{
    if ( mode == Orbit && evt.y != 0 )
    {
        const Ogre::Real dist = (nodeCam->getPosition() - nodeTarget->_getDerivedPosition()).length();
        nodeCam->translate( Ogre::Vector3(0, 0, -evt.y * 0.08f * dist),
                            Ogre::Node::TS_LOCAL );
    }
    return true;
}

bool CameraCtrl::mousePressed( const OgreBites::MouseButtonEvent & evt )
{
    if ( mode == Orbit )
    {
        if ( evt.button == OgreBites::BUTTON_LEFT )
            mOrbiting = true;
        else if ( evt.button == OgreBites::BUTTON_RIGHT )
            mMoving = true;
    }
    if ( mode != Fixed )
    {
        if ( evt.button == OgreBites::BUTTON_LEFT )
            mOrbiting = true;
        StateManager::getSingletonPtr()->setMouseVisible( false );
    }
    return true;
}

bool CameraCtrl::mouseReleased( const OgreBites::MouseButtonEvent & evt )
{
    if ( mode == Orbit )
    {
        if ( evt.button == OgreBites::BUTTON_LEFT )
            mOrbiting = false;
        else if ( evt.button == OgreBites::BUTTON_RIGHT )
            mMoving = false;
    }
    if ( mode != Fixed )
    {
        if ( evt.button == OgreBites::BUTTON_LEFT )
            mOrbiting = false;
        StateManager::getSingletonPtr()->setMouseVisible( true );
    }

    return true;
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
    if ( !nodeCam )
        return 0.0;
    if ( !nodeTarget )
        return 0.0;
    const Ogre::Vector3 offset = nodeCam->getPosition() - nodeTarget->_getDerivedPosition();
    const Ogre::Real d = offset.length();
    return d;
}








