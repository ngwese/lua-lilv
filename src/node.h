#ifndef __NODE_H__
#define __NODE_H__

#include "lua_lilv.h"

typedef struct {
    LilvNode *node;
    bool is_owner;
} node_t;

int node_open(lua_State *L);
int node_new(lua_State *L, LilvNode *c, bool is_owner);
node_t *node_check(lua_State *L, int arg);

#endif // __NODE_H__