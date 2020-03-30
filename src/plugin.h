#ifndef __PLUGIN_H__
#define __PLUGIN_H__

#include "lua_lilv.h"

int plugin_open(lua_State *L);
int plugin_new(lua_State *L, const LilvPlugin *p);

#endif // __PLUGIN_H__