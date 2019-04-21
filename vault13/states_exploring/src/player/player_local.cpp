
#include "player_local.h"

namespace Osp
{

EntityPlayer::EntityPlayer()
    : Player()
{

}

EntityPlayer::~EntityPlayer()
{

}

bool EntityPlayer::local() const
{
    return true;
}

int  EntityPlayer::actionsQty() const
{
    return 0;
}

bool EntityPlayer::action( Action & a )
{
    return false;
}

bool EntityPlayer::keyPressed(const OgreBites::KeyboardEvent& evt)
{
    return false;
}

bool EntityPlayer::keyReleased(const OgreBites::KeyboardEvent& evt)
{

    return false;
}

bool EntityPlayer::touchMoved(const OgreBites::TouchFingerEvent& evt)
{

    return false;
}

bool EntityPlayer::touchPressed(const OgreBites::TouchFingerEvent& evt)
{

    return false;
}

bool EntityPlayer::touchReleased(const OgreBites::TouchFingerEvent& evt)
{

    return false;
}

bool EntityPlayer::mouseMoved(const OgreBites::MouseMotionEvent& evt)
{

    return false;
}

bool EntityPlayer::mouseWheelRolled(const OgreBites::MouseWheelEvent& evt)
{

    return false;
}

bool EntityPlayer::mousePressed(const OgreBites::MouseButtonEvent& evt)
{

    return false;
}

bool EntityPlayer::mouseReleased(const OgreBites::MouseButtonEvent& evt)
{

    return false;
}


}

