
#ifndef __DESIGN_CONSTRUCTION_H_
#define __DESIGN_CONSTRUCTION_H_

#include "Ogre.h"
#include "OgreInput.h"
#include "tech_tree_panel.h"

namespace Entity
{

class Site;
class EntityPart;
class TechTreePanel;

class DesignConstruction: public TechTreePanelCallback
{
public:
    enum MoveMode { TFree=0, TDrag, TRotate };

    DesignConstruction();
    ~DesignConstruction();

    /// Enter/Leave workshop.
    void enter();
    void leave();

    /// TechTreePanel callback implementation.
    void part( const Ogre::String & name );

    /// Input devices callbacks.
    bool frameStarted(const Ogre::FrameEvent& evt);
    bool frameEnded(const Ogre::FrameEvent& evt);
    bool keyPressed(const OgreBites::KeyboardEvent& evt);
    bool keyReleased(const OgreBites::KeyboardEvent& evt);
    bool touchMoved(const OgreBites::TouchFingerEvent& evt);
    bool touchPressed(const OgreBites::TouchFingerEvent& evt);
    bool touchReleased(const OgreBites::TouchFingerEvent& evt);
    bool mouseMoved(const OgreBites::MouseMotionEvent& evt);
    bool mouseWheelRolled(const OgreBites::MouseWheelEvent& evt);
    bool mousePressed(const OgreBites::MouseButtonEvent& evt);
    bool mouseReleased(const OgreBites::MouseButtonEvent& evt);


public:
    /// Depending on camera "-z" axis editing plane is either vertical
    /// or horizontal.
    void cameraPlane( Ogre::Vector3 & x, Ogre::Vector3 & y, Ogre::Vector3 & n );
    /// Mouse ray intersection point with editing plane.
    void mouseInPlane( Ogre::Vector2 & xy );
    /// Absolute position of mouse ray intersection with editing plane.
    void mouseAbs( Ogre::Vector3 & xyz );

    Site          * workshop;
    TechTreePanel * techTreePanel;

    int selectedPartIndex;
    MoveMode moveMode;
    std::vector<EntityPart *> parts;

    Ogre::Real snapDist;
};


}


#endif



