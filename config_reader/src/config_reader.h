
#ifndef __CONFIG_READER_H_
#define __CONFIG_READER_H_

#include <string>

extern "C"
{
    struct lua_State;
    typedef int (* ScriptFunc) (lua_State *L);
}

namespace Config
{

/**
 * @brief The ConfigReader class
 * Configuration file reader. It uses Lua scripting language for config files
 * reading. E.i. it assumes a file is in Lua language reads it and uses Lua tables
 * for storing values.
 */
class ConfigReader
{
public:
    typedef double Float;

    ConfigReader( bool verbose = false );
    ~ConfigReader();

    bool openFile( const char * fname = "./odometryConfig.lua" );
    bool error( const char * * stri ) const;

    bool readFloat(  const char * name, Float & v, Float defValue=0.0 );
    bool readInt(    const char * name, int   & v, int   defValue=0 );
    bool readBool(   const char * name, bool  & v, bool  defValue=true );
    bool readString( const char * name, std::string & stri, const char * defValue = 0 );
    bool pathExists( const char * path );

    void funcRegister( const char * funcName, ScriptFunc func );
    lua_State * luaState();
private:
    class PD;
    PD * pd;
};

}

#endif








