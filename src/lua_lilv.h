#ifndef __LUA_LILV_H__
#define __LUA_LILV_H__

#define LUA_LILV_VERSION    "???"

#ifndef LUA_LILV_API
#define LUA_LILV_API __attribute__ ((visibility ("default")))
#endif

#include "lua.h"
#include "lauxlib.h"

#include "lilv/lilv.h"

LUA_LILV_API int luaopen_lilv_core(lua_State *L);

#endif