#ifndef __PORT_H__
#define __PORT_H__

#include "lua_lilv.h"

int port_open(lua_State *L);
int port_new(lua_State *L, const LilvPlugin *plug, const LilvPort *port);

#endif // __PORT_H__