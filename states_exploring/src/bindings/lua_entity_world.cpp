
#include "lua_entity_part.h"
#include "state_manager.h"
#include "entity_world.h"
#include "config_reader.h"
#include "lua_collision_shapes.h"
#include "lua.hpp"
#include "lua_utils.h"

static const char LIB_NAME[]    = "core";
static const char META_T_NAME[] = "EW";

static int lua_worldCreate( lua_State * L )
{
    Osp::DynamicsWorld * e = StateManager::getSingletonPtr()->getWorld();
    if ( !e )
    {
        lua_pushboolean( L, 0 );
        Ogre::LogManager::getSingletonPtr()->logError( "Failed to retrieve world" );
        return 1;
    }
    Osp::DynamicsWorld * * p = reinterpret_cast<Osp::DynamicsWorld * *>(
                lua_newuserdata( L, sizeof(Osp::DynamicsWorld *)) );
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

static int lua_worldGc( lua_State * L )
{
    Osp::DynamicsWorld * p = *reinterpret_cast<Osp::DynamicsWorld * *>(
                                    lua_touserdata( L, 1 ) );

    // Here do nothing as it is singleton managed
    // in StateManager.
    return 0;
}

static int lua_addPart( lua_State * L )
{
    Osp::DynamicsWorld * w = *reinterpret_cast<Osp::DynamicsWorld * *>(
                lua_touserdata( L, 1 ) );
    Osp::Block * p = *reinterpret_cast<Osp::Block * *>(
                lua_touserdata( L, 2 ) );
    if ( !p )
    {
        Ogre::LogManager::getSingletonPtr()->logError( "No part is specified" );
        return 0;
    }
    w->addEntity( p );
    return 0;
}

static int lua_removePart( lua_State * L )
{
    Osp::DynamicsWorld * w = *reinterpret_cast<Osp::DynamicsWorld * *>(
                lua_touserdata( L, 1 ) );
    Osp::Block * p = *reinterpret_cast<Osp::Block * *>(
                lua_touserdata( L, 2 ) );
    if ( !p )
    {
        Ogre::LogManager::getSingletonPtr()->logError( "No part is specified" );
        return 0;
    }
    w->removeEntity( p );
    return 0;
}

static int lua_addPlanet( lua_State * L )
{
    Osp::DynamicsWorld * w = *reinterpret_cast<Osp::DynamicsWorld * *>(
                lua_touserdata( L, 1 ) );
    Osp::EntityPlanet * p = *reinterpret_cast<Osp::EntityPlanet * *>(
                lua_touserdata( L, 2 ) );
    if ( !p )
    {
        Ogre::LogManager::getSingletonPtr()->logError( "No planet is specified" );
        return 0;
    }
    w->addEntity( p );
    return 0;
}

static int lua_removePlanet( lua_State * L )
{
    Osp::DynamicsWorld * w = *reinterpret_cast<Osp::DynamicsWorld * *>(
                lua_touserdata( L, 1 ) );
    Osp::EntityPlanet * p = *reinterpret_cast<Osp::EntityPlanet * *>(
                lua_touserdata( L, 2 ) );
    if ( !p )
    {
        Ogre::LogManager::getSingletonPtr()->logError( "No planet is specified" );
        return 0;
    }
    w->removeEntity( p );
    return 0;
}

static int lua_frameStarted( lua_State * L )
{
    Osp::DynamicsWorld * w = *reinterpret_cast<Osp::DynamicsWorld * *>(
                lua_touserdata( L, 1 ) );
    const int top = lua_gettop( L );
    const btScalar dt = (top > 1) ? static_cast<btScalar>( lua_tonumber( L, 2 ) ) : 0.1;
    const bool debugDraw = (top > 2) ? (lua_toboolean( L, 3 ) > 0) : false;
    const bool res = w->frameStarted( dt, debugDraw );
    lua_pushboolean( L, res ? 1 : 0 );
    return 1;
}








static const struct luaL_reg META_T_FUNCS[] = {
    { "addPart",      lua_addPart },
    { "removePart",   lua_removePart },
    { "addPlanet",    lua_addPlanet },
    { "removePlanet", lua_removePlanet },
    { "frameStarted", lua_frameStarted },
    { NULL,           NULL },
};

static void worldCreateMeta( lua_State * L )
{
    const int top = lua_gettop( L );
    luaL_newmetatable( L, META_T_NAME );  // create new metatable for file handles
    // file methods
    lua_pushliteral( L, "__index" );
    lua_pushvalue( L, -2 );  // push metatable
    lua_rawset( L, -3 );     // metatable.__index = metatable

    lua_pushliteral( L, "__gc" );
    lua_pushcfunction( L, lua_worldGc );
    lua_rawset( L, -3 );

    luaL_register( L, NULL, META_T_FUNCS );
    // Clean stack.
    lua_settop( L, top );

}

static const struct luaL_reg FUNCS[] =
{
    { "worldCreate", lua_worldCreate },

    { 0, 0 },
};




int luaopen_entityWorld( lua_State * L )
{
    const int top = lua_gettop( L );

    worldCreateMeta( L );
    luaL_register( L, LIB_NAME, FUNCS );

    lua_settop( L, top );
    return 0;
}

