#include "lua_lilv.h"
#include "port.h"
#include "node.h"

typedef struct {
    const LilvPlugin *plugin;
    const LilvPort *port;
} port_t;

//
// prototypes
//

/*
LilvScalePoints *	lilv_port_get_scale_points (const LilvPlugin *plugin, const LilvPort *port)
*/

int port_get_value(lua_State *L);
int port_get_properties(lua_State *L);
int port_has_property(lua_State *L);
int port_supports_event(lua_State *L);
int port_get_symbol(lua_State *L);
int port_get_name(lua_State *L);
int port_get_index(lua_State *L);
int port_get_classes(lua_State *L);
int port_get_range(lua_State *L);
int port_is_a(lua_State *L);

//
// plugin object methods
//
static char *port_modulename = "Port";
static char *port_classname = "lilv.Port";

static luaL_Reg port_methods[] = {
    {"get_value", port_get_value},
    {"get_properties", port_get_properties},
    {"has_property", port_has_property},
    {"supports_event", port_supports_event},
    {"get_symbol", port_get_symbol},
    {"get_name", port_get_name},
    {"get_index", port_get_index},
    {"get_classes", port_get_classes},
    {"get_range", port_get_range},
    {"is_a", port_is_a},
    {NULL, NULL}
};

//
// module functions
//

static luaL_Reg module_functions[] = {
    {NULL, NULL}
};

int port_open(lua_State *L) {
    // module name
    lua_pushstring(L, port_modulename);

    // define class
    luaL_Reg *method = port_methods;

    luaL_newmetatable(L, port_classname); // core, mn, mt
    lua_pushstring(L, "__index");          // core, mn, mt, __index

    lua_newtable(L);                       // core, mn, mt, __index, {}
    lua_pushstring(L, "class");            // core, mn, mt, __index, {}, "class"
    lua_pushstring(L, port_classname);    // core, mn, mt, __index, {}, "class", cn
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

int port_new(lua_State *L, const LilvPlugin *plug, const LilvPort *port) {
    port_t *ud = (port_t *)lua_newuserdata(L, sizeof(port_t));
    ud->plugin = plug;
    ud->port = port;
    luaL_getmetatable(L, port_classname);
    lua_setmetatable(L, -2);
    return 1;
}

//
// implementation
//

static port_t *port_check(lua_State *L) {
    void *ud = luaL_checkudata(L, 1, port_classname);
    luaL_argcheck(L, ud != NULL, 1, "'Port' expected");
    return (port_t *)ud;
}

int port_get_value(lua_State *L) {
    const port_t *p = port_check(L);
    const node_t *predicate = node_check(L, 2);
    LilvNodes *list = (LilvNode *)lilv_port_get_value(p->plugin, p->port, predicate->node);
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

int port_get_properties(lua_State *L) {
    const port_t *p = port_check(L);
    LilvNodes *list = (LilvNode *)lilv_port_get_properties(p->plugin, p->port);
    lua_newtable(L);
    if (list) {
        int index = 1;
        if (lilv_nodes_size(list) > 0) {
            LILV_FOREACH(nodes, i, list) {
                const LilvNode *n = lilv_nodes_get(list, i);
                // NOTE: lilv documentation doesn't indicate if nodes need to be
                // freed, we assume no here.
                node_new(L, (LilvNode *)n, false /* is_owned */);
                lua_seti(L, -2, index++);
            }
        }
    }
    return 1;
}

int port_has_property(lua_State *L) {
    const port_t *p = port_check(L);
    const node_t *prop = node_check(L, 2);
    lua_pushboolean(L, lilv_port_has_property(p->plugin, p->port, prop->node));
    return 1;
}

int port_supports_event(lua_State *L) {
    const port_t *p = port_check(L);
    const node_t *event = node_check(L, 2);
    lua_pushboolean(L, lilv_port_supports_event(p->plugin, p->port, event->node));
    return 1;
}

int port_get_symbol(lua_State *L) {
    const port_t *p = port_check(L);
    LilvNode *s = (LilvNode *)lilv_port_get_symbol(p->plugin, p->port); // FIXME: const correctness
    node_new(L, s, false /* is_owned */);
    return 1;
}

int port_get_name(lua_State *L) {
    const port_t *p = port_check(L);
    LilvNode *s = lilv_port_get_name(p->plugin, p->port);
    if (s) {
        lua_pushstring(L, lilv_node_as_string(s));
    } else {
        lua_pushnil(L);
    }
    lilv_node_free(s);
    return 1;
}

int port_get_index(lua_State *L) {
    const port_t *p = port_check(L);
    uint32_t index = lilv_port_get_index(p->plugin, p->port);
    // convert index to 1s base to match plugin_get_port_by_index
    lua_pushinteger(L, index + 1);
    return 1;
}

int port_get_classes(lua_State *L) {
    const port_t *p = port_check(L);
    const LilvNodes *list = lilv_port_get_classes(p->plugin, p->port);
    lua_newtable(L);
    if (list) {
        int index = 1;
        if (lilv_nodes_size(list) > 0) {
            LILV_FOREACH(nodes, i, list) {
                const LilvNode *n = lilv_nodes_get(list, i);
                node_new(L, (LilvNode *)n, false /* is_owned */);
                lua_seti(L, -2, index++);
            }
        }
    }
    return 1;
}

int port_get_range(lua_State *L) {
    LilvNode *deft, *min, *max;
    const port_t *p = port_check(L);
    lilv_port_get_range(p->plugin, p->port, &deft, &min, &max);
    lua_newtable(L);
    if (deft) {
        lua_pushnumber(L, lilv_node_as_float(deft));
        lilv_node_free(deft);
    } else {
        lua_pushnil(L);
    }
    lua_rawseti(L, -2, 1);
    if (min) {
        lua_pushnumber(L, lilv_node_as_float(min));
        lilv_node_free(min);
    } else {
        lua_pushnil(L);
    }
    lua_rawseti(L, -2, 2);
    if (max) {
        lua_pushnumber(L, lilv_node_as_float(max));
        lilv_node_free(max);
    } else {
        lua_pushnil(L);
    }
    lua_rawseti(L, -2, 3);
    return 1;
}

int port_is_a(lua_State *L) {
    const port_t *p = port_check(L);
    const node_t *n = node_check(L, 2);
    lua_pushboolean(L,lilv_port_is_a(p->plugin, p->port, n->node));
    return 1;
}
