
#include "state_manager.h"
#include "config_reader.h"
#include "lua.hpp"
#include "lua_utils.h"

static const char LIB_NAME[]          = "core";
static const char SOUND_META_T_NAME[] = "SND";

static void luaNotify( lua_State * L, const std::string & callbackName, const std::string & stateName )
{
    const int top = lua_gettop( L );
    lua_pushstring( L, callbackName.c_str() );
    lua_gettable( L, LUA_GLOBALSINDEX );
    if ( lua_isfunction( L, -1 ) == 0 )
    {
        lua_settop( L, top );
        std::string stri = callbackName + " is not defined";
        Ogre::LogManager::getSingletonPtr()->logWarning( stri );
        return;
    }
    lua_pushstring( L, stateName.c_str() );
    const int res = lua_pcall( L, 1, 0, 0 );
    if ( res != 0 )
    {
        reportError( L );
        lua_settop( L, top );
        return;
    }
    lua_settop( L, top );
}

void StateManager::scriptFrameStarted( const Ogre::FrameEvent & evt )
{
    lua_State * L = confReader->luaState();
    if ( !L )
        return;

    static const char funcName[] = "frameStarted";
    const int top = lua_gettop( L );
    lua_pushstring( L, funcName );
    lua_gettable( L, LUA_GLOBALSINDEX );
    if ( lua_isfunction( L, -1 ) == 0 )
    {
        lua_settop( L, top );
        std::string stri = std::string( funcName ) + " is not defined";
        Ogre::LogManager::getSingletonPtr()->logWarning( stri );
        return;
    }
    lua_pushnumber( L, evt.timeSinceLastFrame );
    const int res = lua_pcall( L, 1, 0, 0 );
    if ( res != 0 )
    {
        reportError( L );
        lua_settop( L, top );
        return;
    }
    lua_settop( L, top );
}

void StateManager::stateEntered( const std::string & name )
{
    lua_State * L = confReader->luaState();
    luaNotify( L, "stateEntered", name );
}

void StateManager::stateLeft( const std::string & name )
{
    lua_State * L = confReader->luaState();
    luaNotify( L, "stateLeft", name );
}

void StateManager::statePaused( const std::string & name )
{
    lua_State * L = confReader->luaState();
    luaNotify( L, "statePaused", name );
}

void StateManager::stateResumed( const std::string & name )
{
    lua_State * L = confReader->luaState();
    luaNotify( L, "stateResumed", name );
}



static int lua_log( lua_State * L )
{
    const int top = lua_gettop( L );
    for ( int i=1; i<=top; i++ )
    {
        const char * stri = lua_tostring( L, i );
        Ogre::LogManager::getSingletonPtr()->logMessage( stri );
    }
}

static int lua_warning( lua_State * L )
{
    const int top = lua_gettop( L );
    for ( int i=1; i<=top; i++ )
    {
        const char * stri = lua_tostring( L, i );
        Ogre::LogManager::getSingletonPtr()->logWarning( stri );
    }
}

static int lua_err( lua_State * L )
{
    const int top = lua_gettop( L );
    for ( int i=1; i<=top; i++ )
    {
        const char * stri = lua_tostring( L, i );
        Ogre::LogManager::getSingletonPtr()->logError( stri );
    }
}




static int lua_soundCreate( lua_State * L )
{
    OgreOggSound::OgreOggSoundManager * m =
            OgreOggSound::OgreOggSoundManager::getSingletonPtr();

    const int top = lua_gettop( L );
    // Read parameters.
    const char * soundFile = lua_tostring( L, 1 );
    const char * soundName = lua_tostring( L, 2 );
    const bool   stream    = (top > 2) ? (lua_toboolean( L, 3 ) > 0) : false;
    const bool   loop      = (top > 3) ? (lua_toboolean( L, 4 ) > 0) : false;
    const bool   preBuffer = (top > 4) ? (lua_toboolean( L, 5 ) > 0) : false;

    OgreOggSound::OgreOggISound * s;
    const bool exists = m->hasSound( soundName );
    if ( !exists )
    {
        lua_pushboolean( L, 0 );
        Ogre::LogManager::getSingletonPtr()->logError( "Sound already exists" );
        return 1;
    }
    try
    {
        s = m->createSound( soundName, soundFile, stream, loop, preBuffer, 0, false );
    }
    catch ( ... )
    {
        lua_pushboolean( L, 0 );
        Ogre::LogManager::getSingletonPtr()->logError( "Failed to create sound" );
        return 1;
    }

    OgreOggSound::OgreOggISound * * p = reinterpret_cast<OgreOggSound::OgreOggISound * *>(
                lua_newuserdata( L, sizeof(OgreOggSound::OgreOggISound *)) );
    *p = s;

    // Get metatable.
    luaL_getmetatable( L, SOUND_META_T_NAME );
    if ( !(lua_istable(L, -1) > 0) )
    {
        lua_pushboolean( L, 0 );
        Ogre::LogManager::getSingletonPtr()->logError( "Failed to retrieve metatable" );
        return 1;
    }
    lua_setmetatable( L, -2 );

    return 1;
}

static int lua_soundGc( lua_State * L )
{
    OgreOggSound::OgreOggISound * s = *reinterpret_cast<OgreOggSound::OgreOggISound * *>(
                lua_touserdata( L, 1 ) );
    OgreOggSound::OgreOggSoundManager * m =
            OgreOggSound::OgreOggSoundManager::getSingletonPtr();
    m->destroySound( s );
    return 0;
}

static int lua_soundPlay( lua_State * L )
{
    OgreOggSound::OgreOggISound * s = *reinterpret_cast<OgreOggSound::OgreOggISound * *>(
                lua_touserdata( L, 1 ) );
    const int top = lua_gettop( L );
    const bool immediate = (top > 1) ? (lua_toboolean(L, 2) > 0) : false;
    s->play( immediate );
    return 0;
}

static int lua_soundIsPlaying( lua_State * L )
{
    OgreOggSound::OgreOggISound * s = *reinterpret_cast<OgreOggSound::OgreOggISound * *>(
                lua_touserdata( L, 1 ) );
    const bool res = s->isPlaying();
    lua_pushboolean( L, res ? 1 : 0 );
    return 1;
}

static int lua_soundStop( lua_State * L )
{
    OgreOggSound::OgreOggISound * s = *reinterpret_cast<OgreOggSound::OgreOggISound * *>(
                lua_touserdata( L, 1 ) );
    const int top = lua_gettop( L );
    const bool immediate = (top > 1) ? (lua_toboolean(L, 2) > 0) : false;
    s->stop( immediate );
    return 0;
}

static int lua_soundPause( lua_State * L )
{
    OgreOggSound::OgreOggISound * s = *reinterpret_cast<OgreOggSound::OgreOggISound * *>(
                lua_touserdata( L, 1 ) );
    const int top = lua_gettop( L );
    const bool immediate = (top > 1) ? (lua_toboolean(L, 2) > 0) : false;
    s->pause( immediate );
    return 0;
}

static int lua_soundSetLoop( lua_State * L )
{
    OgreOggSound::OgreOggISound * s = *reinterpret_cast<OgreOggSound::OgreOggISound * *>(
                lua_touserdata( L, 1 ) );
    const int top = lua_gettop( L );
    const bool doLoop = (top > 1) ? (lua_toboolean(L, 2) > 0) : true;
    s->loop( doLoop );
    return 0;
}







static const struct luaL_reg SOUND_META_FUNCS[] = {
    { "play",       lua_soundPlay },
    { "isPlaying",  lua_soundIsPlaying },
    { "stop",       lua_soundStop },
    { "pause",      lua_soundPause },
    { "setLoop",    lua_soundSetLoop },

    { NULL,            NULL },
};

static void soundCreateMeta( lua_State * L )
{
    const int top = lua_gettop( L );
    luaL_newmetatable( L, SOUND_META_T_NAME );  // create new metatable for file handles
    // file methods
    lua_pushliteral( L, "__index" );
    lua_pushvalue( L, -2 );  // push metatable
    lua_rawset( L, -3 );     // metatable.__index = metatable

    lua_pushliteral( L, "__gc" );
    lua_pushcfunction( L, lua_soundGc );
    lua_rawset( L, -3 );

    luaL_register( L, NULL, SOUND_META_FUNCS );
    // Clean stack.
    lua_settop( L, top );

}

static const struct luaL_reg FUNCS[] =
{
    { "log",         lua_log },
    { "warning",     lua_warning },
    { "err",         lua_err },
    { "createSound", lua_soundCreate },

    { 0, 0 },
};





int luaopen_sound( lua_State * L )
{
    soundCreateMeta( L );
    luaL_register( L, LIB_NAME, FUNCS );
}





