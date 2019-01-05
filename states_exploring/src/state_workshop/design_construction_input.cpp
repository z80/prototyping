
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
    return true;
}

bool DesignConstruction::keyReleased(const OgreBites::KeyboardEvent& evt)
{
    return true;
}

bool DesignConstruction::touchMoved(const OgreBites::TouchFingerEvent& evt)
{
    return true;
}

bool DesignConstruction::touchPressed(const OgreBites::TouchFingerEvent& evt)
{
    return true;
}

bool DesignConstruction::touchReleased(const OgreBites::TouchFingerEvent& evt)
{
    return true;
}

bool DesignConstruction::mouseMoved(const OgreBites::MouseMotionEvent& evt)
{
    return true;
}

bool DesignConstruction::mouseWheelRolled(const OgreBites::MouseWheelEvent& evt)
{
    return true;
}

bool DesignConstruction::mousePressed(const OgreBites::MouseButtonEvent& evt)
{
    return true;
}

bool DesignConstruction::mouseReleased(const OgreBites::MouseButtonEvent& evt)
{
    return true;
}




}



