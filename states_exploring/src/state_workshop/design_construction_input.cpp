
#include "design_construction.h"
#include "state_manager.h"
#include "tech_tree_panel.h"
#include "OgreInput.h"


namespace Osp
{

bool DesignConstruction::frameStarted(const Ogre::FrameEvent& evt)
{
    if ( techTreePanel )
    {
        techTreePanel->drawTechPanel( this );
        techTreePanel->drawBackToGamePanel();
        techTreePanel->drawTipPanel();
        techTreePanel->drawMainMenu( designManager );
    }
    return false;
}

bool DesignConstruction::frameEnded(const Ogre::FrameEvent& evt)
{
    return false;
}

bool DesignConstruction::keyPressed(const OgreBites::KeyboardEvent& evt)
{
    if ( moveMode != TFree )
    {
        if ( evt.keysym.sym == 27 )
        {
            moveMode          = TFree;
            selectedBlockIndex = -1;
            return true;
        }
    }
    return false;
}

bool DesignConstruction::keyReleased(const OgreBites::KeyboardEvent& evt)
{
    if ( evt.keysym.sym == 27 )
    {
        if ( moveMode != TFree )
        {
            moveMode = TFree;
            return true;
        }
    }
    if ( selectedBlockIndex >= 0 )
    {
        if ( evt.keysym.sym == 'g' )
        {
            moveMode = TDrag;
            dragStart();
            //StateManager::getSingletonPtr()->setMouseVisible( false );
            return true;
        }
        else if ( evt.keysym.sym == 'r' )
        {
            moveMode = TRotate;
            rotateStart();
            //StateManager::getSingletonPtr()->setMouseVisible( false );
            return true;
        }
        else if ( evt.keysym.sym == 'x' )
        {
            destroy();
            return true;
        }
    }
    return false;
}

bool DesignConstruction::touchMoved(const OgreBites::TouchFingerEvent& evt)
{
    return false;
}

bool DesignConstruction::touchPressed(const OgreBites::TouchFingerEvent& evt)
{
    return false;
}

bool DesignConstruction::touchReleased(const OgreBites::TouchFingerEvent& evt)
{
    return false;
}

bool DesignConstruction::mouseMoved(const OgreBites::MouseMotionEvent& evt)
{
    mouseRaySelection = false;

    if ( techTreePanel->isHovered() )
        return true;
    if ( moveMode == TDrag )
    {
        drag();
        return true;
    }
    else if ( moveMode == TRotate )
    {
        rotate();
        return true;
    }
    else if ( moveMode == TFree )
    {
        // To initiate camera control in the case if button down was accompanied
        // by dragging.
        /*if ( needMouseDown )
        {
            needMouseDown = false;
            StateManager::getSingletonPtr()->getCameraCtrl()->mousePressed( mousePressedEvt );
        }*/
    }
    return false;
}

bool DesignConstruction::mouseWheelRolled(const OgreBites::MouseWheelEvent& evt)
{
    return false;
}

bool DesignConstruction::mousePressed(const OgreBites::MouseButtonEvent& evt)
{
    if ( evt.button != OgreBites::BUTTON_LEFT )
        return false;

    if ( techTreePanel->isHovered() )
        return false;

    if ( moveMode == TFree )
    {
        mouseRaySelection = true;
        mousePressedEvt   = evt;
    }

    const bool res = (moveMode != TFree);
    return res;
}

bool DesignConstruction::mouseReleased( const OgreBites::MouseButtonEvent & evt )
{
    if ( evt.button != OgreBites::BUTTON_LEFT )
        return false;

    if ( techTreePanel->isHovered() )
        return false;

    if ( moveMode == TDrag )
    {
        dragStop();
        moveMode = TFree;
        return true;
    }
    else if ( moveMode == TRotate )
    {
        rotateStop();
        moveMode = TFree;
        return true;
    }
    else if ( mouseRaySelection )
    {
        mouseRaySelection = false;
        if ( moveMode == TFree )
        {
            int index;
            const bool partSelected = trySelect( index );
            if ( partSelected )
            {
                return true;
            }
        }
    }

    const bool res = (moveMode != TFree);
    return res;
}




}



