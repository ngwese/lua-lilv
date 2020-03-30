#include "lua_lilv.h"
#include "plugin_class.h"
#include "node.h"

typedef struct {
    const LilvPluginClass *class;
} plugin_class_t;

//
// prototypes
//

static int plugin_class_get_parent_uri(lua_State *L);
static int plugin_class_get_uri(lua_State *L);
static int plugin_class_get_label(lua_State *L);
static int plugin_class_get_children(lua_State *L);

//
// plugin object methods
//
static char *plugin_class_modulename = "PluginClass";
static char *plugin_class_classname = "lilv.PluginClass";

static luaL_Reg plugin_class_methods[] = {
    {"get_parent_uri", plugin_class_get_parent_uri},
    {"get_uri", plugin_class_get_uri},
    {"get_label", plugin_class_get_label},
    {"get_children", plugin_class_get_children},
    {NULL, NULL}
};

//
// module functions
//

static luaL_Reg module_functions[] = {
    {NULL, NULL}
};

int plugin_class_open(lua_State *L) {
    // module name
    lua_pushstring(L, plugin_class_modulename);

    // define class
    luaL_Reg *method = plugin_class_methods;

    luaL_newmetatable(L, plugin_class_classname); // core, mn, mt
    lua_pushstring(L, "__index");          // core, mn, mt, __index

    lua_newtable(L);                       // core, mn, mt, __index, {}
    lua_pushstring(L, "class");            // core, mn, mt, __index, {}, "class"
    lua_pushstring(L, plugin_class_classname);    // core, mn, mt, __index, {}, "class", cn
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

int plugin_class_new(lua_State *L, const LilvPluginClass *c) {
    plugin_class_t *ud = (plugin_class_t *)lua_newuserdata(L, sizeof(plugin_class_t));
    ud->class = c;
    luaL_getmetatable(L, plugin_class_classname);
    lua_setmetatable(L, -2);
    return 1;
}

//
// implementation
//

static plugin_class_t *plugin_class_check(lua_State *L) {
    void *ud = luaL_checkudata(L, 1, plugin_class_classname);
    luaL_argcheck(L, ud != NULL, 1, "'PluginClass' expected");
    return (plugin_class_t *)ud;
}

static int plugin_class_get_parent_uri(lua_State *L) {
    const plugin_class_t *c = plugin_class_check(L);
    const LilvNode *pc = lilv_plugin_class_get_parent_uri(c->class);
    if (pc != NULL) {
        lua_pushstring(L, lilv_node_as_uri(pc));

        return 1;
    }
    return 0;
}

static int plugin_class_get_uri(lua_State *L) {
    const plugin_class_t *c = plugin_class_check(L);
    //lua_pushstring(L, lilv_node_as_uri(lilv_plugin_class_get_uri(c->class)));
    node_new(L, (LilvNode *)lilv_plugin_class_get_uri(c->class), false /* is_owned */);
    return 1;
}

static int plugin_class_get_label(lua_State *L) {
    const plugin_class_t *c = plugin_class_check(L);
    const LilvNode *n = lilv_plugin_class_get_label(c->class);
    lua_pushstring(L, lilv_node_as_string(n));
    return 1;
}

static int plugin_class_get_children(lua_State *L) {
    const plugin_class_t *c = plugin_class_check(L);
    LilvPluginClasses *list = lilv_plugin_class_get_children(c->class);

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

    lilv_plugin_classes_free(list);
    return num_values;
}



