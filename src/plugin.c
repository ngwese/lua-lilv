#include "lilv/lilv.h"

#include "lua_lilv.h"
#include "node.h"
#include "plugin.h"
#include "plugin_class.h"
#include "port.h"

typedef struct {
    const LilvPlugin *plugin;
} plugin_t;

//
// prototypes
//

static int plugin_verify(lua_State *L);
static int plugin_get_uri(lua_State *L);
static int plugin_get_bundle_uri(lua_State *L);
static int plugin_get_value(lua_State *L);
static int plugin_get_name(lua_State *L);
static int plugin_get_class(lua_State *L);
static int plugin_get_num_ports(lua_State *L);
static int plugin_get_port_by_index(lua_State *L);
static int plugin_get_port_by_symbol(lua_State *L);
static int plugin_get_port_by_designation(lua_State *L);

//
// plugin object methods
//
static char *plugin_modulename = "Plugin";
static char *plugin_classname = "lilv.Plugin";

static luaL_Reg plugin_methods[] = {
    {"verify", plugin_verify},
    {"get_uri", plugin_get_uri},
    {"get_bundle_uri", plugin_get_bundle_uri},
    {"get_value", plugin_get_value},
    {"get_name", plugin_get_name},
    {"get_class", plugin_get_class},
    {"get_num_ports", plugin_get_num_ports},
    {"get_port_by_index", plugin_get_port_by_index},
    {"get_port_by_symbol", plugin_get_port_by_symbol},
    {"get_port_by_designation", plugin_get_port_by_designation},
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
    node_new(L, (LilvNode *)lilv_plugin_get_uri(p->plugin), false /* is_owned */);  // FIXME: const
    return 1;
}

static int plugin_get_bundle_uri(lua_State *L) {
    const plugin_t *p = plugin_check(L);
    node_new(L, (LilvNode *)lilv_plugin_get_bundle_uri(p->plugin), false /* is_owned */); // FIXME: const
    return 1;
}

static int plugin_get_value(lua_State *L) {
    const plugin_t *p = plugin_check(L);
    const node_t *predicate = node_check(L, 2);
    const LilvNodes *list = lilv_plugin_get_value(p->plugin, predicate->node);
    if (list) {
        lua_newtable(L);
        int index = 1;
        if (lilv_nodes_size(list) > 0) {
            LILV_FOREACH(nodes, i, list) {
                const LilvNode *n = lilv_nodes_get(list, i);
                node_new(L, (LilvNode *)n, true /* is_owned */);
                lua_seti(L, -2, index++);
            }
        }
    } else {
        lua_pushnil(L);
    }
    return 1;
}

static int plugin_get_name(lua_State *L) {
    const plugin_t *p = plugin_check(L);
    LilvNode *n = lilv_plugin_get_name(p->plugin);
    lua_pushstring(L, lilv_node_as_string(n));
    lilv_node_free(n);
    //node_new(L, (LilvNode *)lilv_plugin_get_name(p->plugin), true /* is_owned */);
    return 1;
}

static int plugin_get_class(lua_State *L) {
    const plugin_t *p = plugin_check(L);
    const LilvPluginClass *c = lilv_plugin_get_class(p->plugin);
    plugin_class_new(L, c);
    return 1;
}

static int plugin_get_num_ports(lua_State *L) {
    const plugin_t *p = plugin_check(L);
    lua_pushinteger(L, lilv_plugin_get_num_ports(p->plugin));
    return 1;
}

static int plugin_get_port_by_index(lua_State *L) {
    const plugin_t *p = plugin_check(L);
    lua_Integer index = luaL_checkinteger(L, 2);
    const LilvPort *port = lilv_plugin_get_port_by_index(p->plugin, index - 1); // convert lua 1 based index
    if (port) {
        port_new(L, p->plugin, port);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

static int plugin_get_port_by_symbol(lua_State *L) {
    const plugin_t *p = plugin_check(L);
    const node_t *symbol = node_check(L, 2);
    const LilvPort *port = lilv_plugin_get_port_by_symbol(p->plugin, symbol->node);
    if (port) {
        port_new(L, p->plugin, port);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

static int plugin_get_port_by_designation(lua_State *L) {
    const plugin_t *p = plugin_check(L);
    const node_t *port_class = node_check(L, 2);
    const node_t *designation = node_check(L, 3);
    const LilvPort *port = lilv_plugin_get_port_by_designation(p->plugin, port_class->node, designation->node);
    if (port) {
        port_new(L, p->plugin, port);
    } else {
        lua_pushnil(L);
    }
    return 1;
}
