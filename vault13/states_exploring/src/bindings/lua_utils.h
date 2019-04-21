
#ifndef __LUA_UTILS_H_
#define __LUA_UTILS_H_

#include "lua.hpp"
#include "btBulletCollisionCommon.h"
#include "Ogre.h"

bool luaReadVector( lua_State * L, int at, btVector3 & v );
bool luaReadVector( lua_State * L, int at, Ogre::Vector3 & v );
bool luaReadQuat( lua_State * L, int at, btQuaternion & v );
bool luaReadQuat( lua_State * L, int at, Ogre::Quaternion & v );
void reportError( lua_State * L );

#endif



