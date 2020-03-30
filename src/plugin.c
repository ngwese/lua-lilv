#include "lilv/lilv.h"

#include "lua_lilv.h"
#include "plugin.h"

typedef struct {
    const LilvPlugin *plugin;
} plugin_t;

//
// prototypes
//

static int plugin_verify(lua_State *L);
static int plugin_get_uri(lua_State *L);

//
// plugin object methods
//
static char *plugin_modulename = "Plugin";
static char *plugin_classname = "lilv.Plugin";

static luaL_Reg plugin_methods[] = {
    {"verify", plugin_verify},
    {"get_uri", plugin_get_uri},
    {NULL, NULL}
};

//
// module functions
//

static luaL_Reg module_functions[] = {
    {NULL, NULL}
};

int plugin_open(lua_State *L) {
    // module name
    lua_pushstring(L, plugin_modulename);

    // define class
    luaL_Reg *method = plugin_methods;

    luaL_newmetatable(L, plugin_classname); // core, mn, mt
    lua_pushstring(L, "__index");          // core, mn, mt, __index

    lua_newtable(L);                       // core, mn, mt, __index, {}
    lua_pushstring(L, "class");            // core, mn, mt, __index, {}, "class"
    lua_pushstring(L, plugin_classname);    // core, mn, mt, __index, {}, "class", cn
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

int plugin_new(lua_State *L, const LilvPlugin *p) {
    plugin_t *u = (plugin_t *)lua_newuserdata(L, sizeof(plugin_t));
    u->plugin = p;
    luaL_getmetatable(L, plugin_classname);
    lua_setmetatable(L, -2);
    return 1;
}

//
// implementation
//

static plugin_t *plugin_check(lua_State *L) {
    void *ud = luaL_checkudata(L, 1, plugin_classname);
    luaL_argcheck(L, ud != NULL, 1, "'plugin' expected");
    return (plugin_t *)ud;
}

static int plugin_verify(lua_State *L) {
    const plugin_t *p = plugin_check(L);
    lua_pushboolean(L, lilv_plugin_verify(p->plugin));
    return 1;
}

static int plugin_get_uri(lua_State *L) {
    const plugin_t *p = plugin_check(L);
    lua_pushstring(L, lilv_node_as_uri(lilv_plugin_get_uri(p->plugin)));
    return 1;
}