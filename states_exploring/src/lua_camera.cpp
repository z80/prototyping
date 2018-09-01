
#include "lua_camera.h"
#include "state_manager.h"
#include "lua_utils.h"

static const char LIB_NAME[]    = "core";
static const char META_T_NAME[] = "CM";

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


int lua_cameraSetPosition( lua_State * L )
{
    Ogre::Camera * p = *reinterpret_cast<Ogre::Camera * *>(
                                    lua_touserdata( L, 1 ) );
    Ogre::Vector3 at;
    const bool ok = luaReadVector( L, 2, at );
    if ( !ok )
    {
        Ogre::LogManager::getSingletonPtr()->logError( "Need a table with 3 numbers" );
        return 0;
    }
    Ogre::SceneNode * n = p->getParentSceneNode();
    if ( n )
        n->setPosition( at );
    return 0;
}

int lua_cameraGetPosition( lua_State * L )
{
    Ogre::Camera * p = *reinterpret_cast<Ogre::Camera * *>(
                                    lua_touserdata( L, 1 ) );
    Ogre::SceneNode * n = p->getParentSceneNode();
    if ( n )
    {
        const Ogre::Vector3 at = n->getPosition();
        lua_pushnumber( L, at.x );
        lua_pushnumber( L, at.y );
        lua_pushnumber( L, at.z );
        return 3;
    }
    return 0;
}

int lua_cameraSetRotation( lua_State * L )
{
    Ogre::Camera * p = *reinterpret_cast<Ogre::Camera * *>(
                                    lua_touserdata( L, 1 ) );
    Ogre::Quaternion q;
    const bool ok = luaReadQuat( L, 2, q );
    if ( !ok )
    {
        Ogre::LogManager::getSingletonPtr()->logError( "Need a table with 4 numbers" );
        return 0;
    }
    Ogre::SceneNode * n = p->getParentSceneNode();
    if ( n )
        n->setOrientation( q );
    return 0;
}

int lua_cameraGetRotation( lua_State * L )
{
    Ogre::Camera * p = *reinterpret_cast<Ogre::Camera * *>(
                                    lua_touserdata( L, 1 ) );
    Ogre::SceneNode * n = p->getParentSceneNode();
    if ( n )
    {
        const Ogre::Quaternion q = n->getOrientation();
        lua_pushnumber( L, q.w );
        lua_pushnumber( L, q.x );
        lua_pushnumber( L, q.y );
        lua_pushnumber( L, q.z );
        return 4;
    }
    return 0;
}

int lua_cameraGetDirection( lua_State * L )
{
    Ogre::Camera * p = *reinterpret_cast<Ogre::Camera * *>(
                                    lua_touserdata( L, 1 ) );
    Ogre::SceneNode * n = p->getParentSceneNode();
    if ( n )
    {
        const Ogre::Quaternion q = n->getOrientation();
        Ogre::Quaternion a( 0.0, 0.0, 0.0, -1.0 );
        a = q * a * q.Inverse();
        lua_pushnumber( L, a.x );
        lua_pushnumber( L, a.y );
        lua_pushnumber( L, a.z );
        return 3;
    }
    return 0;

}





static const struct luaL_reg META_T_FUNCS[] = {
    { "setPosition",  lua_cameraSetPosition },
    { "setRotation",  lua_cameraSetRotation },
    { "getPosition",  lua_cameraGetPosition },
    { "getRotation",  lua_cameraGetRotation },
    { "getDirection", lua_cameraGetDirection },
    { NULL,           NULL },
};

static void cameraCreateMeta( lua_State * L )
{
    const int top = lua_gettop( L );
    luaL_newmetatable( L, META_T_NAME );  // create new metatable for file handles
    // file methods
    lua_pushliteral( L, "__index" );
    lua_pushvalue( L, -2 );  // push metatable
    lua_rawset( L, -3 );     // metatable.__index = metatable

    lua_pushliteral( L, "__gc" );
    lua_pushcfunction( L, lua_cameraGc );
    lua_rawset( L, -3 );

    luaL_register( L, NULL, META_T_FUNCS );
    // Clean stack.
    lua_settop( L, top );

}

static const struct luaL_reg FUNCS[] =
{
    { "partCreate", lua_cameraCreate },

    { 0, 0 },
};







int luaopen_camera( lua_State * L )
{
    cameraCreateMeta( L );
    luaL_register( L, LIB_NAME, FUNCS );
    return 0;
}




