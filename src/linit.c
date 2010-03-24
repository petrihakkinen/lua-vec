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
	float x = lua_tonumber(L, 1);
	float y = lua_tonumber(L, 2);
  lua_pushvec(L, x, y);
  return 1;
}

// LUA-VEC - getvec function
static int getvec (lua_State *L) {
  const float *vec = luaL_checkvec(L, 1);
  lua_Integer  idx = lua_tointeger(L, 2);
  luaL_argcheck(L, idx >= 1 && idx <= LUA_VEC_SIZE, 2, "invalid vector index");
  lua_pushnumber(L, vec[idx-1]);
  return 1;
}

// LUA_VEC - setvec function
static int setvec (lua_State *L) {
  // This function does not yet work as it should ...
	// TODO: this can't be implemented as a normal function
	// TODO: arguments are passed by value so changing the the value only changes the local copy in stack
	/*
  float       *vec = lua_tovec(L, 1);
  lua_Integer  idx = lua_tointeger(L, 2);
  lua_Number   val = lua_tonumber(L, 3);
  luaL_argcheck(L, idx >= 1 && idx <= LUA_VEC_SIZE, 2, "invalid vector index");
  vec[idx-1] = val;
	*/
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

