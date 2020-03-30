#ifndef __NODE_H__
#define __NODE_H__

#include "lua_lilv.h"

int node_open(lua_State *L);
int node_new(lua_State *L, LilvNode *c, bool is_owner);

#endif // __NODE_H__