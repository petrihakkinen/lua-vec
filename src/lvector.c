/*
** Lua Vectors
** See Copyright Notice in lua.h
*/

#define lvector_c
#define LUA_CORE

#include "lua.h"

#include "lmem.h"
#include "lobject.h"
#include "lstate.h"
#include "lvector.h"


Vector *luaVec_new (lua_State *L, float x, float y, float z, float w) {
  Vector *v;
  v = cast(Vector *, luaM_malloc(L, sizeof(Vector)));
  v->tt = LUA_TVEC;
  v->marked = luaC_white(G(L));
  v->next = G(L)->mainthread->next;
  v->vec[0] = x;
  v->vec[1] = y;
  v->vec[2] = z;
  v->vec[3] = w;
  return v;
}

