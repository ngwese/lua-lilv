#include "lua_lilv.h"
#include "world.h"
#include "plugin.h"

typedef struct {
    LilvWorld *world;
} world_t;

//
// prototypes
//

static int world_new(lua_State *L);
static int world_free(lua_State *L);
static int world_load_all(lua_State *L);
static int world_get_all_plugins(lua_State *L);

//
// world object methods
//

static char *world_modulename = "World";
static char *world_classname = "lilv.World";

static luaL_Reg world_methods[] = {
    {"__gc", world_free},
    {"load_all", world_load_all},
    {"get_all_plugins", world_get_all_plugins},
    {NULL, NULL}
};

//
// module functions
//

static luaL_Reg module_functions[] = {
    {"new", world_new},
    {NULL, NULL}
};

int world_open(lua_State *L) {
    // module name
    lua_pushstring(L, world_modulename);

    // define class
    luaL_Reg *method = world_methods;

    luaL_newmetatable(L, world_classname); // core, mn, mt
    lua_pushstring(L, "__index");          // core, mn, mt, __index

    lua_newtable(L);                       // core, mn, mt, __index, {}
    lua_pushstring(L, "class");            // core, mn, mt, __index, {}, "class"
    lua_pushstring(L, world_classname);    // core, mn, mt, __index, {}, "class", cn
    lua_rawset(L, -3);                     // core, mn ,mt, __index, {}

    for (; method->name; method++) {
        lua_pushstring(L, method->name);
        lua_pushcfunction(L, method->func);
        // methods starting with _ are placed in the metatable, else __index table
        lua_rawset(L, method->name[0] == '_' ? -5 : -3);
    }

    // core, mn, mt, __index, {}
    lua_rawset(L, -3);                    // core, mn, mt

    luaL_setfuncs(L, module_functions, 0);

    // register the module
    lua_rawset(L, -3);

    return 0;
}

//
// implementation
//

static world_t *world_check(lua_State *L) {
    void *ud = luaL_checkudata(L, 1, world_classname);
    luaL_argcheck(L, ud != NULL, 1, "'world' expected");
    return (world_t *)ud;
}

static int world_new(lua_State *L) {
    world_t *ud = (world_t *)lua_newuserdata(L, sizeof(world_t));
    ud->world = lilv_world_new();
    luaL_getmetatable(L, world_classname);
    lua_setmetatable(L, -2);
    return 1;
}

static int world_free(lua_State *L) {
    world_t *w = world_check(L);
    if (w->world != NULL) {
        lilv_world_free(w->world);
        w->world = NULL;
    }
    return 0;
}

static int world_load_all(lua_State *L) {
    world_t *w = world_check(L);
    lilv_world_load_all(w->world);
    return 0;
}

static int world_get_all_plugins(lua_State *L) {
    const world_t *w = world_check(L);
    const LilvPlugins *list = lilv_world_get_all_plugins(w->world);
    int index = 1;
    lua_newtable(L);
    LILV_FOREACH(plugins, i, list) {
        const LilvPlugin *p = lilv_plugins_get(list, i);
        plugin_new(L, p);
        lua_seti(L, -2, index++);
    }
    return 1;
}

