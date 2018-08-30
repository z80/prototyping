
#include "lua_camera.h"
#include "state_manager.h"
#include "camera_ctrl.h"

static const char LIB_NAME[]    = "core";
static const char META_T_NAME[] = "EP";

int lua_cameraCreate( lua_State * L )
{
    Ogre::Camera * c = StateManager::getSingletonPtr()->getCamera();
    Ogre::Camera * * p = reinterpret_cast<Ogre::Camera * *>(
                lua_newuserdata( L, sizeof(Ogre::Camera *)) );
    *p = c;
    // Get metatable.
    luaL_getmetatable( L, META_T_NAME );
    if ( !(lua_istable(L, -1) > 0) )
    {
        lua_pushboolean( L, 0 );
        Ogre::LogManager::getSingletonPtr()->logError( "Failed to retrieve metatable" );
        return 1;
    }
    lua_setmetatable( L, -2 );

    return 1;
}

static int lua_cameraGc( lua_State * L )
{
    Ogre::Camera * p = *reinterpret_cast<Ogre::Camera * *>(
                                    lua_touserdata( L, 1 ) );

    // In the case of camera do nothing. Don't delete as
    // there is only one real camera so far.
    return 0;
}


int lua_cameraSetMode( lua_State * L )
{
    //CameraCtrl::getSingletonPtr()->setMode();
    return 0;
}











int luaopen_camera( lua_State * L )
{
    return 0;
}




