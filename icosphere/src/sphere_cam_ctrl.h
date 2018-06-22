
#ifndef __SPHERE_CAM_CTRL_H_
#define __SPHERE_CAM_CTRL_H_

#include "Ogre.h"
#include "OgreFrameListener.h"
#include "OgreInput.h"

using namespace Ogre;
using namespace OgreBites;

namespace IcoHeightmap
{

class ManualSphere;

class SphereCamCtrl: public InputListener
{
public:
    SphereCamCtrl( SceneNode * sphereNode, ManualSphere * sphere, Real r );
    ~SphereCamCtrl();

    void setSphere( ManualSphere * sphere );
    void setCameraNode( SceneNode * camNode );

    void frameRendered(const Ogre::FrameEvent& evt);
    bool keyPressed(const KeyboardEvent& evt);
    bool keyReleased(const KeyboardEvent& evt);
    bool mouseMoved(const MouseMotionEvent& evt);
    bool mouseWheelRolled(const MouseWheelEvent& evt);
    bool mousePressed(const MouseButtonEvent& evt);
    bool mouseReleased(const MouseButtonEvent& evt);
private:
    class PD;
    PD * pd;
};

}

#endif




