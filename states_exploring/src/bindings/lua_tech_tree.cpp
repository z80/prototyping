
#include "lua_tech_tree.h"
#include "tech_tree.h"
#include "state_manager.h"


static const char LIB_NAME[]    = "core";
static const char META_T_NAME[] = "TT";

static int lua_create( lua_State * L )
{
    Entity::TechTree * e = StateManager::getSingletonPtr()->getTechTree();
    if ( !e )
    {
        lua_pushboolean( L, 0 );
        Ogre::LogManager::getSingletonPtr()->logError( "Failed to retrieve TechTree" );
        return 1;
    }
    Entity::TechTree * * p = reinterpret_cast<Entity::TechTree * *>(
                lua_newuserdata( L, sizeof(Entity::TechTree *)) );
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

static int lua_gc( lua_State * L )
{
    Entity::TechTree * tt = *reinterpret_cast<Entity::TechTree * *>(
                                    lua_touserdata( L, 1 ) );

    // Here do nothing as it is singleton managed
    // in StateManager.
    return 0;
}

static int lua_clear( lua_State * L )
{
    Entity::TechTree * tt = *reinterpret_cast<Entity::TechTree * *>(
                lua_touserdata( L, 1 ) );
    tt->nodes.clear();
    return 0;
}

static int lua_addNode( lua_State * L )
{
    Entity::TechTree * tt = *reinterpret_cast<Entity::TechTree * *>(
                lua_touserdata( L, 1 ) );

    Entity::TechNode n;
    n.enableable = false;
    n.enabled    = false;
    n.name       = "";
    n.iconEnabled  = "icon";
    n.iconDisabled = "icon";
    n.iconHovered  = "icon";
    n.at = Ogre::Vector2::ZERO;

    lua_pushstring( L, "enabled" );
    lua_gettable( L, -2 );
    if ( lua_isboolean( L, -1 ) )
        n.enabled = (lua_toboolean( L, -1 ) != 0);
    lua_pop( L, 1 );

    lua_pushstring( L, "name" );
    lua_gettable( L, -2 );
    if ( lua_isstring( L, -1 ) )
        n.name = lua_tostring( L, -1 );
    lua_pop( L, 1 );

    lua_pushstring( L, "at" );
    lua_gettable( L, -2 );
    if ( lua_istable( L, -1 ) )
    {
        lua_pushstring( L, "x" );
        lua_gettable( L, -2 );
        if ( lua_isnumber( L, -1 ) )
            n.at.x = lua_tonumber( L, -1 );
        lua_pop( L, -1 );
        lua_pushstring( L, "y" );
        lua_gettable( L, -2 );
        if ( lua_isnumber( L, -1 ) )
            n.at.y = lua_tonumber( L, -1 );
        lua_pop( L, -1 );
    }
    lua_pop( L, 1 );

    // Icons
    lua_pushstring( L, "iconEnabled" );
    lua_gettable( L, -2 );
    if ( lua_isstring( L, -1 ) )
        n.iconEnabled = lua_tostring( L, -1 );
    lua_pop( L, 1 );

    lua_pushstring( L, "iconDisabled" );
    lua_gettable( L, -2 );
    if ( lua_isstring( L, -1 ) )
        n.iconDisabled = lua_tostring( L, -1 );
    lua_pop( L, 1 );

    lua_pushstring( L, "iconHovered" );
    lua_gettable( L, -2 );
    if ( lua_isstring( L, -1 ) )
        n.iconHovered = lua_tostring( L, -1 );
    lua_pop( L, 1 );

    tt->nodes.push_back( n );

}




static const struct luaL_reg META_T_FUNCS[] = {
    { "clear",        lua_clear },
    { "addNode",      lua_addNode },
    { NULL,           NULL },
};

static void createMeta( lua_State * L )
{
    const int top = lua_gettop( L );
    luaL_newmetatable( L, META_T_NAME );  // create new metatable for file handles
    // file methods
    lua_pushliteral( L, "__index" );
    lua_pushvalue( L, -2 );  // push metatable
    lua_rawset( L, -3 );     // metatable.__index = metatable

    lua_pushliteral( L, "__gc" );
    lua_pushcfunction( L, lua_gc );
    lua_rawset( L, -3 );

    luaL_register( L, NULL, META_T_FUNCS );
    // Clean stack.
    lua_settop( L, top );

}

static const struct luaL_reg FUNCS[] =
{
    { "techTreeCreate", lua_create },

    { 0, 0 },
};




int luaopen_techTree( lua_State * L )
{
    createMeta( L );
    luaL_register( L, LIB_NAME, FUNCS );
    return 0;
}






