
#ifndef __LUA_COLLISION_SHAPES_H_
#define __LUA_COLLISION_SHAPES_H_

#include "lua.hpp"
#include "btBulletCollisionCommon.h"


int luaopen_btShapes( lua_State * L );

class ShapeWrapper
{
public:
    bool released;
    btCollisionShape * s;

    ShapeWrapper();
    ~ShapeWrapper();
};



#endif


