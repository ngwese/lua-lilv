
#include "lua_lilv.h"
#include "node.h"
#include "plugin.h"
#include "plugin_class.h"
#include "world.h"
#include "port.h"

static const luaL_Reg mod[] = {
    {"node", node_open},
    {"plugin", plugin_open},
    {"plugin_class", plugin_class_open},
    {"world", world_open},
    {"port", port_open},
    {NULL, NULL}
};

static luaL_Reg func[] = {
    {NULL, NULL}
};

LUA_LILV_API int luaopen_lilv_core(lua_State *L) {
    lua_newtable(L);

    for (int i = 0; mod[i].name; i++) {
        mod[i].func(L);
    }

    luaL_setfuncs(L, func, 0);

    lua_pushstring(L, "_VERSION");
    lua_pushstring(L, LUA_LILV_VERSION);
    lua_rawset(L, -3);

    return 1;
}