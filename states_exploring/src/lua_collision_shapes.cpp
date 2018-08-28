
#include "btBulletCollisionCommon.h"
#include "config_reader.h"
#include "lua.hpp"

static const char LIB_NAME[]           = "core";
static const char BULLET_META_T_NAME[] = "BT";

class ShapeSingle
{
public:
    bool released;
    btCollisionShape * s;

    ShapeSingle()
    {
        released = false;
        s = 0;
    }

    ~ShapeSingle()
    {
        if ( !released )
        {
            if ( s )
                delete s;
        }
    }
};

static int lua_btSphereCreate( lua_State * L )
{
    const int top = lua_gettop( L );
    // Read parameters.
    const btScalar r = static_cast<btScalar>( lua_tostring( L, 1 ) );

    ShapeSingle * s = new ShapeSingle();
    s->s = new btSphereShape( r );
    ShapeSingle * * p = reinterpret_cast<ShapeSingle * *>(
                lua_newuserdata( L, sizeof(ShapeSingle *)) );
    *p = s;

    // Get metatable.
    luaL_getmetatable( L, BULLET_META_T_NAME );
    if ( !(lua_istable(L, -1) > 0) )
    {
        lua_pushboolean( L, 0 );
        lua_pushstring( L, "Failed to retrieve metatable" );
    }
    lua_setmetatable( L, -2 );

    return 1;
}

static int lua_btBoxCreate( lua_State * L )
{
    // Read parameters.
    const btScalar x = static_cast<btScalar>( lua_tostring( L, 1 ) );
    const btScalar y = static_cast<btScalar>( lua_tostring( L, 2 ) );
    const btScalar z = static_cast<btScalar>( lua_tostring( L, 3 ) );

    ShapeSingle * s = new ShapeSingle();
    s->s = new btBoxShape( btVector3( x, y, z ) );
    ShapeSingle * * p = reinterpret_cast<ShapeSingle * *>(
                lua_newuserdata( L, sizeof(ShapeSingle *)) );
    *p = s;

    // Get metatable.
    luaL_getmetatable( L, BULLET_META_T_NAME );
    if ( !(lua_istable(L, -1) > 0) )
    {
        lua_pushboolean( L, 0 );
        lua_pushstring( L, "Failed to retrieve metatable" );
    }
    lua_setmetatable( L, -2 );

    return 1;
}


static int lua_btCapsuleCreate( lua_State * L )
{
    const int top = lua_gettop( L );
    // Read parameters.
    const btScalar r = static_cast<btScalar>( lua_tostring( L, 1 ) );
    const btScalar h = static_cast<btScalar>( lua_tostring( L, 2 ) );

    ShapeSingle * s = new ShapeSingle();
    s->s = new btCapsuleShape( r, h );
    ShapeSingle * * p = reinterpret_cast<ShapeSingle * *>(
                lua_newuserdata( L, sizeof(ShapeSingle *)) );
    *p = s;

    // Get metatable.
    luaL_getmetatable( L, BULLET_META_T_NAME );
    if ( !(lua_istable(L, -1) > 0) )
    {
        lua_pushboolean( L, 0 );
        lua_pushstring( L, "Failed to retrieve metatable" );
    }
    lua_setmetatable( L, -2 );

    return 1;
}

static int lua_simpleShapeGc( lua_State * L )
{
    ShapeSingle * s = *reinterpret_cast<ShapeSingle * *>(
                lua_touserdata( L, 1 ) );
    delete s;
    return 0;
}











