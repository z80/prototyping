
#include "lua_collision_shapes.h"
#include "config_reader.h"
#include "lua.hpp"

static void deleteCompound( btCollisionShape * s )
{
    btCompoundShape * cs = dynamic_cast<btCompoundShape *>( s );
    if ( !cs )
        return;
    const int qty = cs->getNumChildShapes();
    for ( int i=0; i<qty; i++ )
    {
        btCollisionShape * ss = cs->getChildShape( i );
        delete ss;
    }
}

ShapeWrapper::ShapeWrapper()
{
    released = false;
    s = 0;
}

ShapeWrapper::~ShapeWrapper()
{
    if ( !released )
    {
        if ( s )
        {
            const int type = s->getShapeType();
            if ( type == COMPOUND_SHAPE_PROXYTYPE )
                deleteCompound( s );
            delete s;
        }
    }
}




static const char LIB_NAME[]                = "core";
static const char SIMPLESHAPE_META_T_NAME[] = "SS";
static const char COMPSHAPE_META_T_NAME[]   = "CS";

static bool luaReadVector( lua_State * L, int at, btVector3 & v )
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

static bool luaReadQuat( lua_State * L, int at, btQuaternion & v )
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

static int lua_btSphereCreate( lua_State * L )
{
    const int top = lua_gettop( L );
    // Read parameters.
    const btScalar r = static_cast<btScalar>( lua_tonumber( L, 1 ) );

    ShapeWrapper * s = new ShapeWrapper();
    s->s = new btSphereShape( r );
    ShapeWrapper * * p = reinterpret_cast<ShapeWrapper * *>(
                lua_newuserdata( L, sizeof(ShapeWrapper *)) );
    *p = s;

    // Get metatable.
    luaL_getmetatable( L, SIMPLESHAPE_META_T_NAME );
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
    const btScalar x = static_cast<btScalar>( lua_tonumber( L, 1 ) );
    const btScalar y = static_cast<btScalar>( lua_tonumber( L, 2 ) );
    const btScalar z = static_cast<btScalar>( lua_tonumber( L, 3 ) );

    ShapeWrapper * s = new ShapeWrapper();
    s->s = new btBoxShape( btVector3( x, y, z ) );
    ShapeWrapper * * p = reinterpret_cast<ShapeWrapper * *>(
                lua_newuserdata( L, sizeof(ShapeWrapper *)) );
    *p = s;

    // Get metatable.
    luaL_getmetatable( L, SIMPLESHAPE_META_T_NAME );
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
    const btScalar r = static_cast<btScalar>( lua_tonumber( L, 1 ) );
    const btScalar h = static_cast<btScalar>( lua_tonumber( L, 2 ) );

    ShapeWrapper * s = new ShapeWrapper();
    s->s = new btCapsuleShape( r, h );
    ShapeWrapper * * p = reinterpret_cast<ShapeWrapper * *>(
                lua_newuserdata( L, sizeof(ShapeWrapper *)) );
    *p = s;

    // Get metatable.
    luaL_getmetatable( L, SIMPLESHAPE_META_T_NAME );
    if ( !(lua_istable(L, -1) > 0) )
    {
        lua_pushboolean( L, 0 );
        lua_pushstring( L, "Failed to retrieve metatable" );
    }
    lua_setmetatable( L, -2 );

    return 1;
}

static int lua_btCompoundCreate( lua_State * L )
{
    const int top = lua_gettop( L );

    ShapeWrapper * s = new ShapeWrapper();
    s->s = new btCompoundShape();
    ShapeWrapper * * p = reinterpret_cast<ShapeWrapper * *>(
                lua_newuserdata( L, sizeof(ShapeWrapper *)) );
    *p = s;

    // Get metatable.
    luaL_getmetatable( L, COMPSHAPE_META_T_NAME );
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
    ShapeWrapper * s = *reinterpret_cast<ShapeWrapper * *>(
                lua_touserdata( L, 1 ) );
    delete s;
    return 0;
}

static int lua_compShapeGc( lua_State * L )
{
    ShapeWrapper * s = *reinterpret_cast<ShapeWrapper * *>(
                lua_touserdata( L, 1 ) );
    if ( !s->released )
    {
        btCompoundShape * cs = reinterpret_cast<btCompoundShape *>( s->s );
        const int qty = cs->getNumChildShapes();
        for ( int i=0; i<qty; i++ )
        {
            btCollisionShape * ss = cs->getChildShape( i );
            delete ss;
        }
    }
    delete s;
    return 0;
}

static int lua_compShapeAddChild( lua_State * L )
{
    ShapeWrapper * s = *reinterpret_cast<ShapeWrapper * *>(
                lua_touserdata( L, 1 ) );
    ShapeWrapper * c = *reinterpret_cast<ShapeWrapper * *>(
                lua_touserdata( L, 2 ) );
    const int top = lua_gettop( L );
    btVector3    at;
    btQuaternion q;
    if ( top > 2 )
        luaReadVector( L, 3, at );
    if ( top > 3 )
        luaReadQuat( L, 3, q );
    btCompoundShape * cs = reinterpret_cast<btCompoundShape *>( s->s );
    cs->addChildShape( btTransform( q, at ), c->s );
    c->released = true;

    return 0;
}








static void simpleShapeCreateMeta( lua_State * L )
{
    const int top = lua_gettop( L );
    luaL_newmetatable( L, SIMPLESHAPE_META_T_NAME );  // create new metatable for file handles
    // file methods
    lua_pushliteral( L, "__index" );
    lua_pushvalue( L, -2 );  // push metatable
    lua_rawset( L, -3 );     // metatable.__index = metatable

    lua_pushliteral( L, "__gc" );
    lua_pushcfunction( L, lua_simpleShapeGc );
    lua_rawset( L, -3 );

    // Clean stack.
    lua_settop( L, top );
}

static const struct luaL_reg COMPSHAPE_META_FUNCS[] = {
    { "addChild", lua_compShapeAddChild },

    { NULL,       NULL },
};


static void compShapeCreateMeta( lua_State * L )
{
    const int top = lua_gettop( L );
    luaL_newmetatable( L, COMPSHAPE_META_T_NAME );  // create new metatable for file handles
    // file methods
    lua_pushliteral( L, "__index" );
    lua_pushvalue( L, -2 );  // push metatable
    lua_rawset( L, -3 );     // metatable.__index = metatable

    lua_pushliteral( L, "__gc" );
    lua_pushcfunction( L, lua_compShapeGc );
    lua_rawset( L, -3 );

    luaL_register( L, NULL, COMPSHAPE_META_FUNCS );
    // Clean stack.
    lua_settop( L, top );
}


static const struct luaL_reg FUNCS[] =
{
    { "btSphere",   lua_btSphereCreate },
    { "btBox",      lua_btBoxCreate },
    { "btCapsule",  lua_btCapsuleCreate },
    { "btCompound", lua_btCompoundCreate },

    { 0, 0 },
};





int luaopen_btShapes( lua_State * L )
{
    simpleShapeCreateMeta( L );
    compShapeCreateMeta( L );
    luaL_register( L, LIB_NAME, FUNCS );
}









