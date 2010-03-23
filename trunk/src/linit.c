/*
** $Id: linit.c,v 1.14.1.1 2007/12/27 13:02:25 roberto Exp $
** Initialization of libraries for lua.c
** See Copyright Notice in lua.h
*/


#define linit_c
#define LUA_LIB

#include "lua.h"

#include "lualib.h"
#include "lauxlib.h"


static const luaL_Reg lualibs[] = {
  {"", luaopen_base},
  {LUA_LOADLIBNAME, luaopen_package},
  {LUA_TABLIBNAME, luaopen_table},
  {LUA_IOLIBNAME, luaopen_io},
  {LUA_OSLIBNAME, luaopen_os},
  {LUA_STRLIBNAME, luaopen_string},
  {LUA_MATHLIBNAME, luaopen_math},
  {LUA_DBLIBNAME, luaopen_debug},
  {NULL, NULL}
};

// LUA-VEC - test function for creating a new vec
static int vec (lua_State *L) {
  lua_pushvec(L, 1.0f, 2.0f);
  return 1;
}

// LUA-VEC - getvec function
static int getvec (lua_State *L) {
  // Probably needs some more type checking
  const float *vec = lua_tocvec(L, 1);
  lua_Integer  idx = lua_tointeger(L, 2);
  if (idx >= 0 && idx < 2)
    lua_pushnumber(L, vec[idx]);
  else
    lua_pushnil(L);
  return 1;
}

// LUA_VEC - setvec function
static int setvec (lua_State *L) {
  // This function does not yet work as it should ...
  float       *vec = lua_tovec(L, 1);
  lua_Integer  idx = lua_tointeger(L, 2);
  lua_Number   val = lua_tonumber(L, 3);
  vec[idx] = val;
  return 0;
}

LUALIB_API void luaL_openlibs (lua_State *L) {
  const luaL_Reg *lib = lualibs;
  for (; lib->func; lib++) {
    lua_pushcfunction(L, lib->func);
    lua_pushstring(L, lib->name);
    lua_call(L, 1, 0);
  }
	
  // LUA-VEC
  lua_register(L, "vec", vec);
  lua_register(L, "getvec", getvec);
  lua_register(L, "setvec", setvec);
}

