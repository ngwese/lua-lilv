#include "lua_lilv.h"
#include "node.h"
#include "plugin.h"
#include "plugin_class.h"
#include "world.h"

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
static int world_get_plugin_class(lua_State *L);
static int world_get_plugin_classes(lua_State *L);
// node constructors
static int world_new_uri(lua_State *L);
static int world_new_file_uri(lua_State *L);
static int world_new_string(lua_State *L);
static int world_new_int(lua_State *L);
static int world_new_float(lua_State *L);
static int world_new_bool(lua_State *L);

//
// world object methods
//

static char *world_modulename = "World";
static char *world_classname = "lilv.World";

static luaL_Reg world_methods[] = {
    {"__gc", world_free},
    {"load_all", world_load_all},
    {"get_all_plugins", world_get_all_plugins},
    {"get_plugin_class", world_get_plugin_class},
    {"get_plugin_classes", world_get_plugin_classes},
    {"new_uri", world_new_uri},
    {"new_file_uri", world_new_file_uri},
    {"new_string", world_new_string},
    {"new_int", world_new_int},
    {"new_float", world_new_float},
    {"new_bool", world_new_bool},
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
    // printf("world_free %p\n", w->world);
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

static int world_get_plugin_class(lua_State *L) {
    const world_t *w = world_check(L);
    plugin_class_new(L, lilv_world_get_plugin_class(w->world));
    return 1;
}

static int world_get_plugin_classes(lua_State *L) {
    const world_t *w = world_check(L);
    const LilvPluginClasses *list = lilv_world_get_plugin_classes(w->world);

    int num_values = 0;
    int index = 1;

    if (lilv_plugin_classes_size(list) > 0) {
        lua_newtable(L);
        LILV_FOREACH(plugin_classes, i, list) {
            const LilvPluginClass *cc = lilv_plugin_classes_get(list, i);
            plugin_class_new(L, cc);
            lua_seti(L, -2, index++);
        }
        num_values = 1;
    }

    return num_values;
}

static int world_new_uri(lua_State *L) {
    world_t *w = world_check(L);
    const char *uri = luaL_checkstring(L, -1);
    LilvNode *n = lilv_new_uri(w->world, uri);
    if (n) {
      node_new(L, n, true /* is_owner */);
      return 1;
    }
    return 0;
}

static int world_new_file_uri(lua_State *L) {
    world_t *w = world_check(L);
    const char *host = luaL_checkstring(L, -2);
    const char *path = luaL_checkstring(L, -1);
    LilvNode *n = lilv_new_file_uri(w->world, host, path);
    if (n) {
      node_new(L, n, true /* is_owner */);
      return 1;
    }
    return 0;
}

static int world_new_string(lua_State *L) {
    world_t *w = world_check(L);
    const char *s = luaL_checkstring(L, -1);
    LilvNode *n = lilv_new_string(w->world, s);
    if (n) {
      node_new(L, n, true /* is_owner */);
      return 1;
    }
    return 0;
}

static int world_new_int(lua_State *L) {
    world_t *w = world_check(L);
    lua_Integer i = luaL_checkinteger(L, -1);
    LilvNode *n = lilv_new_int(w->world, i);
    node_new(L, n, true /* is_owner */);
    return 1;
}

static int world_new_float(lua_State *L) {
    world_t *w = world_check(L);
    lua_Number f = luaL_checknumber(L, -1);
    LilvNode *n = lilv_new_float(w->world, f);
    if (n) {
      node_new(L, n, true /* is_owner */);
      return 1;
    }
    return 0;
}

static int world_new_bool(lua_State *L) {
    world_t *w = world_check(L);
    LilvNode *n = lilv_new_bool(w->world, lua_toboolean(L, -1));
    if (n) {
      node_new(L, n, true /* is_owner */);
      return 1;
    }
    return 0;
}
