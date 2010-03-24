/*
** Vector math library
*/


#include <stdlib.h>
#include <math.h>

#define lveclib_c
#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"

static int vec_new (lua_State *L) {
	float x = lua_tonumber(L, 1);
	float y = lua_tonumber(L, 2);
	float z = lua_tonumber(L, 3);
	float w = lua_tonumber(L, 4);
  lua_pushvec(L, x, y, z, w);
  return 1;
}

static int vec_dot3 (lua_State *L) {
  const float* v1 = luaL_checkvec(L, 1);
  const float* v2 = luaL_checkvec(L, 2);
  lua_pushnumber(L, v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2]);
  return 1;
}

static int vec_dot4 (lua_State *L) {
  const float* v1 = luaL_checkvec(L, 1);
  const float* v2 = luaL_checkvec(L, 2);
  lua_pushnumber(L, v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2] + v1[3]*v2[3]);
  return 1;
}

static int vec_cross (lua_State *L) {
  const float* v1 = luaL_checkvec(L, 1);
  const float* v2 = luaL_checkvec(L, 2);
  lua_pushvec(L, v1[1] * v2[2] - v1[2] * v2[1], v1[2] * v2[0] - v1[0] * v2[2], v1[0] * v2[1] - v1[1] * v2[0], 0.0f);
  return 1;
}

static int vec_length3 (lua_State *L) {
  const float* v = luaL_checkvec(L, 1);
  lua_pushnumber(L, sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]));
  return 1;
}

static int vec_length4 (lua_State *L) {
  const float* v = luaL_checkvec(L, 1);
  lua_pushnumber(L, sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2] + v[3]*v[3]));
  return 1;
}

static int vec_normalize3 (lua_State *L) {
  const float* v = luaL_checkvec(L, 1);
  float s = 1.0f / sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
  lua_pushvec(L, v[0]*s, v[1]*s, v[2]*s, 0.0f);
  return 1;
}

static int vec_normalize4 (lua_State *L) {
  const float* v = luaL_checkvec(L, 1);
  float s = 1.0f / sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2] + v[3]*v[3]);
  lua_pushvec(L, v[0]*s, v[1]*s, v[2]*s, v[3]*s);
  return 1;
}


static const luaL_Reg veclib[] = {
  {"new",        vec_new},
  {"dot3",       vec_dot3},
  {"dot4",       vec_dot4},
  {"cross",      vec_cross},
  {"length3",    vec_length3},
  {"length4",    vec_length4},
  {"normalize3", vec_normalize3},
  {"normalize4", vec_normalize4},
  {NULL, NULL}
};


/*
** Open veclib
*/
LUALIB_API int luaopen_vec (lua_State *L) {
  luaL_register(L, LUA_VECLIBNAME, veclib);
  
  // numeric constants
  lua_pushvec(L, 0, 0, 0, 0);
  lua_setfield(L, -2, "zero");
  lua_pushvec(L, 1, 1, 1, 1);
  lua_setfield(L, -2, "one");
  return 1;
}

