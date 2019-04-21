
#ifndef __ENTITY_PLAYER_H_
#define __ENTITY_PLAYER_H_

#include "player.h"

namespace Osp
{

/**
* Here button presses, mouse clicks, network data commands, etc. are
* converted into game commands.
**/

class EntityPlayer: public Player
{
public:
    EntityPlayer();
    ~EntityPlayer();

    bool local() const;

    int  actionsQty() const;
    bool action( Action & a );

    virtual bool keyPressed(const OgreBites::KeyboardEvent& evt);
    virtual bool keyReleased(const OgreBites::KeyboardEvent& evt);
    virtual bool touchMoved(const OgreBites::TouchFingerEvent& evt);
    virtual bool touchPressed(const OgreBites::TouchFingerEvent& evt);
    virtual bool touchReleased(const OgreBites::TouchFingerEvent& evt);
    virtual bool mouseMoved(const OgreBites::MouseMotionEvent& evt);
    virtual bool mouseWheelRolled(const OgreBites::MouseWheelEvent& evt);
    virtual bool mousePressed(const OgreBites::MouseButtonEvent& evt);
    virtual bool mouseReleased(const OgreBites::MouseButtonEvent& evt);
};


}



#endif


