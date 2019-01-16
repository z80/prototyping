
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
    if ( techTreePanel->isHovered() )
        return true;
    if ( moveMode == TDrag )
    {
        drag();
        return true;
    }
    else if ( moveMode == TRotate )
    {
        return true;
    }
    else if ( moveMode == TFree )
    {
        // To initiate camera control in the case if button down was accompanied
        // by dragging.
        StateManager::getSingletonPtr()->getCameraCtrl()->mousePressed( mousePressedEvt );
    }
    return false;
}

bool DesignConstruction::mouseWheelRolled(const OgreBites::MouseWheelEvent& evt)
{
    return false;
}

bool DesignConstruction::mousePressed(const OgreBites::MouseButtonEvent& evt)
{
    mouseDown = true;
    if ( techTreePanel->isHovered() )
        return false;

    mousePressedEvt = evt;

    const bool res = (moveMode != TFree);
    return res;
}

bool DesignConstruction::mouseReleased( const OgreBites::MouseButtonEvent & evt )
{
    mouseDown = false;

    if ( techTreePanel->isHovered() )
        return false;

    if ( ( mousePressedEvt.x == evt.x ) && ( mousePressedEvt.y == evt.y ) )
    {
        if ( ( moveMode == TDrag ) || (moveMode == TRotate) )
        {
            moveMode = TFree;
            setPivotsVisible( false );
            return true;
        }
        else if ( moveMode == TFree )
        {
            int index;
            const bool partSelected = trySelect( index );
            if ( partSelected )
            {
                moveMode = TDrag;
                setPivotsVisible( true );
                return true;
            }
        }
    }

    const bool res = (moveMode != TFree);
    return res;
}




}



