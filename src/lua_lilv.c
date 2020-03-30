
#include "lua_lilv.h"
#include "world.h"

static const luaL_Reg mod[] = {
    {"world", world_open},
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