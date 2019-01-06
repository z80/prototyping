
#include "config_reader.h"

#include "lua.hpp"
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <boost/algorithm/string.hpp>

namespace Config
{

class ConfigReader::PD
{
public:
    lua_State * L;
    std::string lastError;
    bool error;
    bool verbose;
    std::string              fullPath,
                             paramName;
    std::vector<std::string> path;
    int top;

    void luaClose()
    {
        if ( L )
        {
            lua_close( L );
            L = 0;
        }
    }

    PD( bool verbose )
    {
        L   = 0;
        top = -1;
        this->verbose = verbose;
        error = false;
    }

    ~PD()
    {
        luaClose();
    }

    void log( const char * path, const char * err )
    {
        if ( verbose )
        {
            if ( path )
                std::cout << path;
            if ( path && err )
                std::cout << " " << err << std::endl;
            else if ( err )
                std::cout << err << std::endl;
        }
    }

    bool intoTheFolder( const char * valuePath, bool canBeTable=false )
    {
        if ( !L )
            return false;
        path.clear();
        fullPath = valuePath;
        boost::split( path, fullPath, boost::is_any_of( "/." ) );
        top = lua_gettop( L );
        const size_t sz = path.size();
        const size_t paramIndex = sz-1;
        for ( size_t i=0; i<sz; i++ )
        {
            const char * stri = path[i].c_str();
            lua_pushstring( L, stri );
            if ( i == 0 )
                lua_gettable( L, LUA_GLOBALSINDEX );
            else
                lua_gettable( L, -2 );
            const int t = lua_type( L, -1 );
            if ( i >= paramIndex )
            {
                if ( ( ( t == LUA_TTABLE ) && (!canBeTable) ) || ( t == LUA_TNIL ) )
                {
                    lua_settop( L, top );
                    log( valuePath, " does not exist" );
                    return false;
                }
            }
            else if ( t != LUA_TTABLE )
            {
                lua_settop( L, top );
                log( valuePath, " does not exist" );
                return false;
            }
        }

        return true;
    }

    void toTheRoot()
    {
        lua_settop( L, top );
    }

};




ConfigReader::ConfigReader( bool verbose )
{
    pd = new PD( verbose );
    pd->L = luaL_newstate();
    luaL_openlibs( pd->L );
}

ConfigReader::~ConfigReader()
{
    delete pd;
}

bool ConfigReader::openFile( const char * fname )
{
    pd->top = lua_gettop( pd->L );
    int err = luaL_dofile( pd->L, fname );
    if ( err != 0 )
    {
        std::ostringstream out;
        int top = lua_gettop( pd->L );
        for ( int i=1; i<=top; i++ )
        {
            const char * stri = lua_tostring( pd->L, i );
            out << stri << std::endl;
        }
        lua_pop( pd->L, top );

        pd->lastError = out.str();

        return false;
    }

    return true;
}

bool ConfigReader::error( const char * * stri ) const
{
    *stri = pd->lastError.c_str();
    return pd->error;
}

bool ConfigReader::readFloat( const char * name, ConfigReader::Float & v, ConfigReader::Float defValue )
{
    const bool pathOk = pd->intoTheFolder( name );
    if ( !pathOk )
    {
        v = defValue;
        return false;
    }
    bool res;
    if ( lua_type( pd->L, -1 ) == LUA_TNUMBER )
    {
        v = static_cast<Float>( lua_tonumber( pd->L, -1 ) );
        res = true;
    }
    else
    {
        v = defValue;
        res = false;
        pd->log( name, " is not a number" );
    }
    pd->toTheRoot();
    return res;
}

bool ConfigReader::readInt( const char * name, int & v, int defValue )
{
    const bool pathOk = pd->intoTheFolder( name );
    if ( !pathOk )
    {
        v = defValue;
        return false;
    }
    bool res;
    if ( lua_type( pd->L, -1 ) == LUA_TNUMBER )
    {
        v = static_cast<int>( lua_tonumber( pd->L, -1 ) );
        res = true;
    }
    else
    {
        v = defValue;
        res = false;
        pd->log( name, " is not a number" );
    }
    pd->toTheRoot();
    return res;
}

bool ConfigReader::readBool(  const char * name, bool  & v, bool  defValue )
{
    const bool pathOk = pd->intoTheFolder( name );
    if ( !pathOk )
    {
        v = defValue;
        return false;
    }
    bool res;
    if ( lua_type( pd->L, -1 ) == LUA_TBOOLEAN )
    {
        v = ( lua_toboolean( pd->L, -1 ) > 0 );
        res = true;
    }
    else
    {
        v = defValue;
        res = false;
        pd->log( name, " is not boolean" );
    }
    pd->toTheRoot();
    return res;
}

bool ConfigReader::readString( const char * name, std::string & stri, const char * defValue )
{
    const bool pathOk = pd->intoTheFolder( name );
    if ( !pathOk )
    {
        stri = ( defValue ) ? defValue : "";
        return false;
    }
    bool res;
    if ( lua_type( pd->L, -1 ) == LUA_TSTRING )
    {
        stri = lua_tostring( pd->L, -1 );
        res = true;
    }
    else
    {
        stri = ( defValue ) ? defValue : "";
        res = false;
        pd->log( name, " is not a string" );
    }
    pd->toTheRoot();
    return res;
}

bool ConfigReader::pathExists( const char * path )
{
    const bool res = pd->intoTheFolder( path, true );
    pd->toTheRoot();

    return res;
}

void ConfigReader::funcRegister( const char * funcName, ScriptFunc func )
{
    lua_pushstring( pd->L, funcName );
    lua_pushcfunction( pd->L, func );
    lua_settable( pd->L, LUA_GLOBALSINDEX );
}

lua_State * ConfigReader::luaState()
{
    return pd->L;
}




}








