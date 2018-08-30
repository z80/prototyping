
#include "lua_utils.h"

bool luaReadVector( lua_State * L, int at, btVector3 & v )
{
    const bool table = ( lua_istable( L, at ) > 0 );
    if ( !table )
        return false;
    lua_pushinteger( L, 1 );
    lua_gettable( L, at );
    v.setX( lua_tonumber( L, -1 ) );
    lua_pop( L, 1 );

    lua_pushinteger( L, 2 );
    lua_gettable( L, at );
    v.setY( lua_tonumber( L, -1 ) );
    lua_pop( L, 1 );

    lua_pushinteger( L, 3 );
    lua_gettable( L, at );
    v.setZ( lua_tonumber( L, -1 ) );
    lua_pop( L, 1 );

    return true;
}

bool luaReadVector( lua_State * L, int at, Ogre::Vector3 & v )
{
    const bool table = ( lua_istable( L, at ) > 0 );
    if ( !table )
        return false;
    lua_pushinteger( L, 1 );
    lua_gettable( L, at );
    v.x = lua_tonumber( L, -1 );
    lua_pop( L, 1 );

    lua_pushinteger( L, 2 );
    lua_gettable( L, at );
    v.y = lua_tonumber( L, -1 );
    lua_pop( L, 1 );

    lua_pushinteger( L, 3 );
    lua_gettable( L, at );
    v.z = lua_tonumber( L, -1 );
    lua_pop( L, 1 );

    return true;
}

bool luaReadQuat( lua_State * L, int at, btQuaternion & v )
{
    const bool table = ( lua_istable( L, at ) > 0 );
    if ( !table )
        return false;
    lua_pushinteger( L, 1 );
    lua_gettable( L, at );
    v.setW( lua_tonumber( L, -1 ) );
    lua_pop( L, 1 );

    lua_pushinteger( L, 2 );
    lua_gettable( L, at );
    v.setX( lua_tonumber( L, -1 ) );
    lua_pop( L, 1 );

    lua_pushinteger( L, 3 );
    lua_gettable( L, at );
    v.setY( lua_tonumber( L, -1 ) );
    lua_pop( L, 1 );

    lua_pushinteger( L, 4 );
    lua_gettable( L, at );
    v.setZ( lua_tonumber( L, -1 ) );
    lua_pop( L, 1 );

    return true;
}

bool luaReadQuat( lua_State * L, int at, Ogre::Quaternion & v )
{
    const bool table = ( lua_istable( L, at ) > 0 );
    if ( !table )
        return false;
    lua_pushinteger( L, 1 );
    lua_gettable( L, at );
    v.w = lua_tonumber( L, -1 );
    lua_pop( L, 1 );

    lua_pushinteger( L, 2 );
    lua_gettable( L, at );
    v.x = lua_tonumber( L, -1 );
    lua_pop( L, 1 );

    lua_pushinteger( L, 3 );
    lua_gettable( L, at );
    v.y = lua_tonumber( L, -1 );
    lua_pop( L, 1 );

    lua_pushinteger( L, 4 );
    lua_gettable( L, at );
    v.z = lua_tonumber( L, -1 );
    lua_pop( L, 1 );

    return true;
}

void reportError( lua_State * L )
{
    const int top = lua_gettop( L );
    for ( int i=1; i<=top; i++ )
    {
        const char * err = lua_tostring( L, i );
        Ogre::LogManager::getSingletonPtr()->logError( err );
    }
}



