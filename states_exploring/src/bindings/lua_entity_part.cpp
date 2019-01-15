
#include "lua_entity_part.h"
#include "entity_part.h"
#include "config_reader.h"
#include "lua_collision_shapes.h"
#include "lua.hpp"
#include "lua_utils.h"

static const char LIB_NAME[]    = "core";
static const char META_T_NAME[] = "EP";

static int lua_partCreate( lua_State * L )
{
    Osp::Block * e = new Osp::Block();
    Osp::Block * * p = reinterpret_cast<Osp::Block * *>(
                lua_newuserdata( L, sizeof(Osp::Block *)) );
    *p = e;
    // Get metatable.
    luaL_getmetatable( L, META_T_NAME );
    if ( !(lua_istable(L, -1) > 0) )
    {
        delete e;
        lua_pushboolean( L, 0 );
        Ogre::LogManager::getSingletonPtr()->logError( "Failed to retrieve metatable" );
        return 1;
    }
    lua_setmetatable( L, -2 );

    return 1;
}

static int lua_partGc( lua_State * L )
{
    Osp::Block * p = *reinterpret_cast<Osp::Block * *>(
                                    lua_touserdata( L, 1 ) );

    delete p;
    return 0;
}

static int lua_setMesh( lua_State * L )
{
    Osp::Block * p = *reinterpret_cast<Osp::Block * *>(
                lua_touserdata( L, 1 ) );
    const char * meshName = lua_tostring( L, 2 );
    bool res = p->setEntity( meshName );
    lua_pushboolean( L, res ? 1 : 0 );
    return 1;
}

static int lua_setMaterial( lua_State * L )
{
    Osp::Block * p = *reinterpret_cast<Osp::Block * *>(
                lua_touserdata( L, 1 ) );
    const char * materialName = lua_tostring( L, 2 );
    bool res = p->setEntity( materialName );
    lua_pushboolean( L, res ? 1 : 0 );
    return 1;
}

static int lua_setCollisionShape( lua_State * L )
{
    Osp::Block * p = *reinterpret_cast<Osp::Block * *>(
                lua_touserdata( L, 1 ) );

    const int top = lua_gettop( L );
    if ( top < 2 )
    {
        lua_pushboolean( L, 0 );
        return 1;
    }

    ShapeWrapper * sw = *reinterpret_cast<ShapeWrapper * *>(
                lua_touserdata( L, 2 ) );

    p->setCollisionShape( sw->s );
    sw->released = true;

    lua_pushboolean( L, 1 );

    return 1;
}

static int lua_setMass( lua_State * L )
{
    Osp::Block * p = *reinterpret_cast<Osp::Block * *>(
                lua_touserdata( L, 1 ) );

    const btScalar m = static_cast<btScalar>( lua_tonumber( L, 2 ) );
    p->mass = m;

    return 0;
}

static int lua_setInertia( lua_State * L )
{
    Osp::Block * p = *reinterpret_cast<Osp::Block * *>(
                lua_touserdata( L, 1 ) );

    Ogre::Vector3 I;
    const bool iExists = luaReadVector( L, 2, I );
    if ( !iExists )
    {
        lua_pushboolean( L, 0 );
        Ogre::LogManager::getSingletonPtr()->logError( "No inertia vector specified" );
        return 1;
    }
    p->setInertia( I );

    lua_pushboolean( L, 1 );
    return 1;
}

static int lua_initDynamics( lua_State * L )
{
    Osp::Block * p = *reinterpret_cast<Osp::Block * *>(
                lua_touserdata( L, 1 ) );
    p->initDynamics();

    return 0;
}

static int lua_applyForce( lua_State * L )
{
    Osp::Block * p = *reinterpret_cast<Osp::Block * *>(
                lua_touserdata( L, 1 ) );
    Ogre::Vector3 at;
    const bool atExists = luaReadVector( L, 2, at );
    if ( !atExists )
    {
        lua_pushboolean( L, 0 );
        Ogre::LogManager::getSingletonPtr()->logError( "No position vector specified" );
        return 1;
    }
    Ogre::Vector3 f;
    const bool fExists = luaReadVector( L, 3, f );
    if ( !fExists )
    {
        lua_pushboolean( L, 0 );
        Ogre::LogManager::getSingletonPtr()->logError( "No force vector specified" );
        return 1;
    }
    p->applyForce( at, f );

    lua_pushboolean( L, 1 );
    return 1;
}

static int lua_applyTorque( lua_State * L )
{
    Osp::Block * p = *reinterpret_cast<Osp::Block * *>(
                lua_touserdata( L, 1 ) );
    Ogre::Vector3 T;
    const bool tExists = luaReadVector( L, 2, T );
    if ( !tExists )
    {
        lua_pushboolean( L, 0 );
        Ogre::LogManager::getSingletonPtr()->logError( "No torque vector specified" );
        return 1;
    }
    p->applyTorque( T );

    lua_pushboolean( L, 1 );
    return 1;
}

static int lua_setPosition( lua_State * L )
{
    Osp::Block * p = *reinterpret_cast<Osp::Block * *>(
                lua_touserdata( L, 1 ) );
    Ogre::Vector3 at;
    const bool atExists = luaReadVector( L, 2, at );
    if ( !atExists )
    {
        lua_pushboolean( L, 0 );
        Ogre::LogManager::getSingletonPtr()->logError( "No position vector specified" );
        return 1;
    }
    p->setR( at );

    lua_pushboolean( L, 1 );
    return 1;
}

static int lua_setRotation( lua_State * L )
{
    Osp::Block * p = *reinterpret_cast<Osp::Block * *>(
                lua_touserdata( L, 1 ) );
    Ogre::Quaternion q;
    const bool qExists = luaReadQuat( L, 2, q );
    if ( !qExists )
    {
        lua_pushboolean( L, 0 );
        Ogre::LogManager::getSingletonPtr()->logError( "No quaternion specified" );
        return 1;
    }
    p->setQ( q );

    lua_pushboolean( L, 1 );
    return 1;
}







static const struct luaL_reg META_T_FUNCS[] = {
    { "setMesh",      lua_setMesh },
    { "setMaterial",  lua_setMaterial },
    { "setShape",     lua_setCollisionShape },
    { "setMass",      lua_setMass },
    { "setInertia",   lua_setInertia },
    { "initDynamics", lua_initDynamics },
    { "applyForce",   lua_applyForce },
    { "applyTorque",  lua_applyTorque },
    { "setPosition",  lua_setPosition },
    { "setRotation",  lua_setRotation },
    { NULL,           NULL },
};

static void partCreateMeta( lua_State * L )
{
    const int top = lua_gettop( L );
    luaL_newmetatable( L, META_T_NAME );  // create new metatable for file handles
    // file methods
    lua_pushliteral( L, "__index" );
    lua_pushvalue( L, -2 );  // push metatable
    lua_rawset( L, -3 );     // metatable.__index = metatable

    lua_pushliteral( L, "__gc" );
    lua_pushcfunction( L, lua_partGc );
    lua_rawset( L, -3 );

    luaL_register( L, NULL, META_T_FUNCS );
    // Clean stack.
    lua_settop( L, top );

}

static const struct luaL_reg FUNCS[] =
{
    { "partCreate", lua_partCreate },

    { 0, 0 },
};




int luaopen_entityPart( lua_State * L )
{
    const int top = lua_gettop( L );

    partCreateMeta( L );
    luaL_register( L, LIB_NAME, FUNCS );

    lua_settop( L, top );
    return 0;
}

