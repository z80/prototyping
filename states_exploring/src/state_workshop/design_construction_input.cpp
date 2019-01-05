
#include "design_construction.h"
#include "tech_tree_panel.h"
#include "OgreInput.h"


namespace Entity
{

bool DesignConstruction::frameStarted(const Ogre::FrameEvent& evt)
{
    if ( techTreePanel )
    {
        techTreePanel->drawTechPanel( this );
        techTreePanel->drawBackToGamePanel();
    }
    return true;
}

bool DesignConstruction::frameEnded(const Ogre::FrameEvent& evt)
{
    return true;
}

bool DesignConstruction::keyPressed(const OgreBites::KeyboardEvent& evt)
{
    if ( evt.keysym.sym == 1 )
    {
        moveMode = TFree;
        return true;
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
    return false;
}

bool DesignConstruction::mouseWheelRolled(const OgreBites::MouseWheelEvent& evt)
{
    return false;
}

bool DesignConstruction::mousePressed(const OgreBites::MouseButtonEvent& evt)
{
    if ( ( moveMode == TDrag ) || (moveMode == TRotate) )
    {
        moveMode = TFree;
        return true;
    }
    else if ( moveMode == TFree )
    {

        return true;
    }
    return false;
}

bool DesignConstruction::mouseReleased(const OgreBites::MouseButtonEvent& evt)
{
    return true;
}




}



