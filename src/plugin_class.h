#ifndef __PLUGIN_CLASS_H__
#define __PLUGIN_CLASS_H__

#include "lua_lilv.h"

int plugin_class_open(lua_State *L);
int plugin_class_new(lua_State *L, const LilvPluginClass *c);

#endif // __PLUGIN_CLASS_H__