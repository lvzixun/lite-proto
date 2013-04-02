#ifndef  _LLP_LUA_H_
#define  _LLP_LUA_H_

#include "../llp.h"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

// open env
int llpL_open(lua_State* L, struct llp_env* env);
// push a message obj with no gc function (the lm must is freed at c) 
void lua_pushlm(lua_State* L, struct llp_mes* lm);
// pop a message obj (the lm not is freed at c, the lm has gc function at lua)
struct llp_mes* lua_tolm(lua_State* L, int idx);

#endif