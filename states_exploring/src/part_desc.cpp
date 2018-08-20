
#include "part_desc.h"
#include <vector>
#include <algorithm>

extern "C"
{
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

namespace Desc
{

PartDesc::PartDesc()
{
    level = 0;
}

PartDesc::~PartDesc()
{

}

PartDesc::PartDesc( const PartDesc & inst )
{
    *this = inst;
}

const PartDesc & PartDesc::operator=( const PartDesc & inst )
{
    if ( this != &inst )
    {
        parts       = inst.parts;
        connections = inst.connections;
        params      = inst.params;
        mass        = inst.mass;
        inertia     = inst.inertia;

        name        = inst.name;
        desc        = inst.desc;
        group       = inst.group;
        icon        = inst.icon;
        level       = inst.level;

    }
    return *this;
}

void PartDesc::addParam( const std::string & name, Ogre::Real value )
{
    params.push_back( Parameter( name, value ) );
    std::push_heap( params.begin(), params.end(), Parameter::Lt );
}

bool PartDesc::param( const std::string & name, Ogre::Real & value )
{
    std::vector<Parameter>::const_iterator it =
            std::lower_bound( params.begin(), params.end(), name, Parameter::Lt );
    if ( it == params.end() )
        return false;
    value = (*it).value;
    return true;
}

void PartDesc::addSound( const std::string & name, const std::string & fileName )
{
    sounds.push_back( Sound( name, fileName ) );
    std::push_heap( sounds.begin(), sounds.end(), Sound::Lt );
}

void PartDesc::finalize()
{
    std::sort_heap( params.begin(), params.end(), Parameter::Lt );
    std::sort_heap( sounds.begin(), sounds.end(), Sound::Lt );
}














static bool readVector( lua_State * L, int ind, Ogre::Vector3 & v )
{
    const bool isT = ( lua_istable( L, ind ) != 0 );
    if ( isT )
    {
        lua_pushinteger( L, 1 );
        lua_gettable( L, ind );
        const Ogre::Real x = lua_tonumber( L, -1 );
        lua_pop( L, 1 );

        lua_pushinteger( L, 2 );
        lua_gettable( L, ind );
        const Ogre::Real y = lua_tonumber( L, -1 );
        lua_pop( L, 1 );

        lua_pushinteger( L, 3 );
        lua_gettable( L, ind );
        const Ogre::Real z = lua_tonumber( L, -1 );
        lua_pop( L, 1 );

        v.x = x;
        v.y = y;
        v.z = z;
    }
    else
    {
        const Ogre::Real x = lua_tonumber( L, ind );
        const Ogre::Real y = lua_tonumber( L, ind+1 );
        const Ogre::Real z = lua_tonumber( L, ind+2 );
        v.x = x;
        v.y = y;
        v.z = z;
    }

    return true;
}




static int connPt_create( lua_State * L )
{
    ConnectionPoint * * p = reinterpret_cast<ConnectionPoint * *>(
                lua_newuserdata( L, sizeof( ConnectionPoint * ) ) );
    *p = new ConnectionPoint();
    luaL_getmetatable( L, CONN_PT_META );
    lua_setmetatable( L, -2 );
    return 1;
}

static int connPt_gc( lua_State * L )
{
    ConnectionPoint * p = *reinterpret_cast<ConnectionPoint * *>(
                lua_touserdata( L, 1 ) );
    delete p;
    return 0;
}

static int connPt_setAt( lua_State * L )
{
    ConnectionPoint * p = *reinterpret_cast<ConnectionPoint * *>(
                lua_touserdata( L, 1 ) );
    readVector( L, 2, p->at );
    return 0;
}

static int connPt_setN( lua_State * L )
{
    ConnectionPoint * p = *reinterpret_cast<ConnectionPoint * *>(
                lua_touserdata( L, 1 ) );
    readVector( L, 2, p->dir );
    return 0;
}





void luaopen_partdesc( lua_State * L )
{

}




}





















