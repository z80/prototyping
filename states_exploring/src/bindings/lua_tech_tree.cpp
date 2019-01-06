
#include "lua_tech_tree.h"
#include "tech_tree.h"
#include "state_manager.h"
#include "lua_utils.h"

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

static int lua_clearNodes( lua_State * L )
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

    const bool hasArgument = ( lua_istable( L, -1 ) > 0 );
    if ( !hasArgument )
    {
        lua_pushboolean( L, 0 );
        return 1;
    }

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

    lua_pushstring( L, "parents" );
    lua_gettable( L, -2 );
    if ( lua_istable( L, -1 ) )
    {
        int ind = 1;
        while ( true )
        {
            lua_pushinteger( L, ind );
            lua_gettable( L, -2 );
            bool needBrake;
            if ( lua_isstring( L, -1 ) )
            {
                const char * name = lua_tostring( L, -1 );
                n.parents.push_back( name );
                needBrake = false;
            }
            else
                needBrake = true;
            lua_pop( L, 1 );
            if ( needBrake )
                break;
            ind += 1;
        }
    }
    lua_pop( L, 1 );

    lua_pushstring( L, "at" );
    lua_gettable( L, -2 );
    if ( lua_istable( L, -1 ) )
    {
        lua_pushinteger( L, 1 );
        lua_gettable( L, -2 );
        if ( lua_isnumber( L, -1 ) )
            n.at.x = lua_tonumber( L, -1 );
        lua_pop( L, 1 );
        lua_pushinteger( L, 2 );
        lua_gettable( L, -2 );
        if ( lua_isnumber( L, -1 ) )
            n.at.y = lua_tonumber( L, -1 );
        lua_pop( L, 1 );
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

    // return "true".
    lua_pushboolean( L, 1 );
    return 1;
}

static int lua_clearParts( lua_State * L )
{
    Entity::TechTree * tt = *reinterpret_cast<Entity::TechTree * *>(
                lua_touserdata( L, 1 ) );
    tt->partDescs.clear();
    return 0;
}

static bool readConnection( lua_State * L, Entity::ConnectionDesc & c );

static int lua_addPart( lua_State * L )
{
    Entity::TechTree * tt = *reinterpret_cast<Entity::TechTree * *>(
                lua_touserdata( L, 1 ) );

    Entity::PartDesc p;
    p.name        = "";
    p.description = "";
    p.tooltip     = "";
    p.neededNode  = "";
    p.category    = "";
    p.icon        = "";

    p.iconHandle  = -1;

    const bool hasArgument = ( lua_istable( L, -1 ) > 0 );
    if ( !hasArgument )
    {
        lua_pushboolean( L, 0 );
        return 1;
    }

    lua_pushstring( L, "name" );
    lua_gettable( L, -2 );
    if ( lua_isstring( L, -1 ) )
        p.name = lua_tostring( L, -1 );
    lua_pop( L, 1 );

    lua_pushstring( L, "description" );
    lua_gettable( L, -2 );
    if ( lua_isstring( L, -1 ) )
        p.description = lua_tostring( L, -1 );
    lua_pop( L, 1 );

    lua_pushstring( L, "tooltip" );
    lua_gettable( L, -2 );
    if ( lua_isstring( L, -1 ) )
        p.tooltip = lua_tostring( L, -1 );
    lua_pop( L, 1 );

    lua_pushstring( L, "neededNode" );
    lua_gettable( L, -2 );
    if ( lua_isstring( L, -1 ) )
        p.neededNode = lua_tostring( L, -1 );
    lua_pop( L, 1 );

    lua_pushstring( L, "category" );
    lua_gettable( L, -2 );
    if ( lua_isstring( L, -1 ) )
        p.category = lua_tostring( L, -1 );
    lua_pop( L, 1 );

    lua_pushstring( L, "icon" );
    lua_gettable( L, -2 );
    if ( lua_isstring( L, -1 ) )
        p.icon = lua_tostring( L, -1 );
    lua_pop( L, 1 );


    lua_pushstring( L, "connections" );
    lua_gettable( L, -2 );
    if ( lua_istable( L, -1 ) )
    {
        int ind = 1;
        while ( true )
        {
            lua_pushinteger( L, ind );
            lua_gettable( L, -2 );
            bool doBreak;
            if ( lua_istable( L, -1 ) )
            {
                // We are in "connection" table now.
                // Need to read it.
                Entity::ConnectionDesc c;
                const bool connOk = readConnection( L, c );
                if ( connOk )
                    p.connections.push_back( c );
                doBreak = false;
            }
            else
                doBreak = true;
            lua_pop( L, 1 );
            if ( doBreak )
                break;
            ind += 1;
        }
    }
    lua_pop( L, 1 );

    tt->partDescs.push_back( p );

    lua_pushboolean( L, 1 );
    return 1;
}

static int lua_enableNode( lua_State * L )
{
    Entity::TechTree * tt = *reinterpret_cast<Entity::TechTree * *>(
                lua_touserdata( L, 1 ) );

    const char * name = lua_tostring( L, 2 );
    const bool res = tt->enableNode( "name" );

    lua_pushboolean( L, res ? 1 : 0 );
    return 1;
}

static int lua_clearCategories( lua_State * L )
{
    Entity::TechTree * tt = *reinterpret_cast<Entity::TechTree * *>(
                lua_touserdata( L, 1 ) );

    tt->panelContent.clear();
    return 0;
}

static int lua_addCategory( lua_State * L )
{
    Entity::TechTree * tt = *reinterpret_cast<Entity::TechTree * *>(
                lua_touserdata( L, 1 ) );

    Entity::CategoryDesc c;
    c.name        = "";
    c.description = "";
    c.tooltip     = "";
    c.icon        = "";

    const bool hasArgument = ( lua_istable( L, -1 ) > 0 );
    if ( !hasArgument )
    {
        lua_pushboolean( L, 0 );
        return 1;
    }

    lua_pushstring( L, "name" );
    lua_gettable( L, -2 );
    if ( lua_isstring( L, -1 ) )
        c.name = lua_tostring( L, -1 );
    lua_pop( L, 1 );

    lua_pushstring( L, "description" );
    lua_gettable( L, -2 );
    if ( lua_isstring( L, -1 ) )
        c.description = lua_tostring( L, -1 );
    lua_pop( L, 1 );

    lua_pushstring( L, "tooltip" );
    lua_gettable( L, -2 );
    if ( lua_isstring( L, -1 ) )
        c.tooltip = lua_tostring( L, -1 );
    lua_pop( L, 1 );

    lua_pushstring( L, "icon" );
    lua_gettable( L, -2 );
    if ( lua_isstring( L, -1 ) )
        c.icon = lua_tostring( L, -1 );
    lua_pop( L, 1 );

    tt->panelContent.push_back( c );

    lua_pushboolean( L, 1 );
    return 1;
}



static const struct luaL_reg META_T_FUNCS[] = {
    { "clearNodes", lua_clearNodes },
    { "addNode",    lua_addNode },
    { "clearParts", lua_clearParts },
    { "addPart",    lua_addPart },
    { "enableNode", lua_enableNode },
    { "clearCategories", lua_clearCategories },
    { "addCategory", lua_addCategory },
    { NULL,         NULL },
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
    const int top = lua_gettop( L );
    createMeta( L );
    luaL_register( L, LIB_NAME, FUNCS );
    lua_settop( L, top );
    return 0;
}






static bool readConnection( lua_State * L, Entity::ConnectionDesc & c )
{
    lua_pushstring( L, "r" );
    lua_gettable( L, -2 );
    if ( lua_istable( L, -1 ) )
        luaReadVector( L, -1, c.r );
    lua_pop( L, 1 );

    lua_pushstring( L, "q" );
    lua_gettable( L, -2 );
    if ( lua_istable( L, -1 ) )
        luaReadQuat( L, -1, c.q );
    lua_pop( L, 1 );

    lua_pushstring( L, "anySurface" );
    lua_gettable( L, -2 );
    if ( lua_isboolean( L, -1 ) )
        c.anySurface = ( lua_toboolean( L, -1) != 0 );
    lua_pop( L, 1 );

    lua_pushstring( L, "spin" );
    lua_gettable( L, -2 );
    if ( lua_isboolean( L, -1 ) )
        c.spin = ( lua_toboolean( L, -1) != 0 );
    lua_pop( L, 1 );

    lua_pushstring( L, "tilt" );
    lua_gettable( L, -2 );
    if ( lua_isboolean( L, -1 ) )
        c.tilt = ( lua_toboolean( L, -1) != 0 );
    lua_pop( L, 1 );

    return true;
}

