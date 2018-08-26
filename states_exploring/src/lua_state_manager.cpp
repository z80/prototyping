
#include "state_manager.h"
#include "config_reader.h"
#include "lua.hpp"

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


int lua_log( lua_State * L )
{
    const int top = lua_gettop( L );
    for ( int i=1; i<=top; i++ )
    {
        const char * stri = lua_tostring( L, i );
        Ogre::LogManager::getSingletonPtr()->logMessage( stri );
    }
}

int lua_warning( lua_State * L )
{
    const int top = lua_gettop( L );
    for ( int i=1; i<=top; i++ )
    {
        const char * stri = lua_tostring( L, i );
        Ogre::LogManager::getSingletonPtr()->logWarning( stri );
    }
}

int lua_err( lua_State * L )
{
    const int top = lua_gettop( L );
    for ( int i=1; i<=top; i++ )
    {
        const char * stri = lua_tostring( L, i );
        Ogre::LogManager::getSingletonPtr()->logError( stri );
    }
}




int lua_soundCreate( lua_State * L )
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
        lua_pushstring( L, "Sound already exists" );
        return 2;
    }
    try
    {
        s = m->createSound( soundName, soundFile, stream, loop, preBuffer, 0, false );
    }
    catch ( ... )
    {
        lua_pushboolean( L, 0 );
        lua_pushstring( L, "Failed to create sound" );
    }

    OgreOggSound::OgreOggISound * * p = reinterpret_cast<OgreOggSound::OgreOggISound * *>(
                lua_newuserdata( L, sizeof(OgreOggSound::OgreOggISound *)) );
    *p = s;

    // Get metatable.
    luaL_getmetatable( L, SOUND_META_T_NAME );
    if ( !(lua_istable(L, -1) > 0) )
    {
        lua_pushboolean( L, 0 );
        lua_pushstring( L, "Failed to retrieve metatable" );
    }
    lua_setmetatable( L, -2 );

    return 1;
}

int lua_soundGc( lua_State * L )
{
    OgreOggSound::OgreOggISound * s = *reinterpret_cast<OgreOggSound::OgreOggISound * *>(
                lua_touserdata( L, 1 ) );
    OgreOggSound::OgreOggSoundManager * m =
            OgreOggSound::OgreOggSoundManager::getSingletonPtr();
    m->destroySound( s );
    return 0;
}

int lua_soundPlay( lua_State * L )
{
    OgreOggSound::OgreOggISound * s = *reinterpret_cast<OgreOggSound::OgreOggISound * *>(
                lua_touserdata( L, 1 ) );
    const int top = lua_gettop( L );
    const bool immediate = (top > 1) ? (lua_toboolean(L, 2) > 0) : false;
    s->play( immediate );
    return 0;
}

int lua_soundIsPlaying( lua_State * L )
{
    OgreOggSound::OgreOggISound * s = *reinterpret_cast<OgreOggSound::OgreOggISound * *>(
                lua_touserdata( L, 1 ) );
    const bool res = s->isPlaying();
    lua_pushboolean( L, res ? 1 : 0 );
    return 1;
}

int lua_soundStop( lua_State * L )
{
    OgreOggSound::OgreOggISound * s = *reinterpret_cast<OgreOggSound::OgreOggISound * *>(
                lua_touserdata( L, 1 ) );
    const int top = lua_gettop( L );
    const bool immediate = (top > 1) ? (lua_toboolean(L, 2) > 0) : false;
    s->stop( immediate );
    return 0;
}

int lua_soundPause( lua_State * L )
{
    OgreOggSound::OgreOggISound * s = *reinterpret_cast<OgreOggSound::OgreOggISound * *>(
                lua_touserdata( L, 1 ) );
    const int top = lua_gettop( L );
    const bool immediate = (top > 1) ? (lua_toboolean(L, 2) > 0) : false;
    s->pause( immediate );
    return 0;
}











