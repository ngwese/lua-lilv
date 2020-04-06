#include "lua_lilv.h"
#include "node.h"

//
// prototypes
//

static int node_free(lua_State *L);
static int node_tostring(lua_State *L);
static int node_as_string(lua_State *L);
static int node_as_integer(lua_State *L);
static int node_as_number(lua_State *L);
static int node_equals(lua_State *L);
static int node_duplicate(lua_State *L);

//
// plugin object methods
//
static char *node_modulename = "Node";
static char *node_classname = "lilv.Node";

static luaL_Reg node_methods[] = {
    {"__gc", node_free},
    {"__tostring", node_tostring},
    {"__eq", node_equals},
    {"duplicate", node_duplicate},
    {"as_string", node_as_string},
    {"as_integer", node_as_integer},
    {"as_number", node_as_number},
    {NULL, NULL}
};

//
// module functions
//

static luaL_Reg module_functions[] = {
    {NULL, NULL}
};

int node_open(lua_State *L) {
    // module name
    lua_pushstring(L, node_modulename);

    // define class
    luaL_Reg *method = node_methods;

    luaL_newmetatable(L, node_classname); // core, mn, mt
    lua_pushstring(L, "__index");          // core, mn, mt, __index

    lua_newtable(L);                       // core, mn, mt, __index, {}
    lua_pushstring(L, "class");            // core, mn, mt, __index, {}, "class"
    lua_pushstring(L, node_classname);    // core, mn, mt, __index, {}, "class", cn
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

int node_new(lua_State *L, LilvNode *n, bool is_owner) {
    node_t *ud = (node_t *)lua_newuserdata(L, sizeof(node_t));
    ud->node = n;
    ud->is_owner = is_owner;
    luaL_getmetatable(L, node_classname);
    lua_setmetatable(L, -2);
    return 1;
}

//
// implementation
//

node_t *node_check(lua_State *L, int arg) {
    void *ud = luaL_checkudata(L, arg, node_classname);
    luaL_argcheck(L, ud != NULL, arg, "'Node' expected");
    return (node_t *)ud;
}

static int node_free(lua_State *L) {
    node_t *n = node_check(L, 1);
    if (n->is_owner && n->node) {
        lilv_node_free(n->node);
    }
    return 0;
}

static int node_tostring(lua_State *L) {
    char buf[512];
    node_t *n = node_check(L, 1);
    LilvNode *node = n->node;

    if (lilv_node_is_uri(node)) {
        sprintf(buf, "(uri): %s", lilv_node_as_uri(node));
        goto finish;
    }

    if (lilv_node_is_string(node)) {
        sprintf(buf, "(str): %s", lilv_node_as_string(node));
        goto finish;
    }

    if (lilv_node_is_blank(node)) {
        sprintf(buf, "(blank): %s", lilv_node_as_blank(node));
        goto finish;
    }

    if (lilv_node_is_float(node)) {
        sprintf(buf, "(float): %f", lilv_node_as_float(node));
        goto finish;
    }

    if (lilv_node_is_int(node)) {
        sprintf(buf, "(int): %d", lilv_node_as_int(node));
        goto finish;
    }

    if (lilv_node_is_bool(node)) {
        sprintf(buf, "(bool): %d", lilv_node_as_bool(node));
        goto finish;
    }

    if (lilv_node_is_literal(node)) {
        sprintf(buf, "(literal): %p", lua_touserdata(L, 1));
        goto finish;
    }

    // unrecognized type
    sprintf(buf, "(?): %p", lua_touserdata(L, 1));

finish:
    lua_pushfstring(L, "%s %s", node_classname, buf);
    return 1;
}

static int node_as_string(lua_State *L) {
    node_t *n = node_check(L, 1);
    LilvNode *node = n->node;

    if (lilv_node_is_uri(node)) {
        lua_pushstring(L, lilv_node_as_uri(node));
        goto finish;
    }

    if (lilv_node_is_string(node)) {
        lua_pushstring(L, lilv_node_as_string(node));
        goto finish;
    }

    if (lilv_node_is_float(node)) {
        lua_pushfstring(L, "%f", lilv_node_as_float(node));
        goto finish;
    }

    if (lilv_node_is_int(node)) {
        lua_pushfstring(L, "%d", lilv_node_as_int(node));
        goto finish;
    }

    if (lilv_node_is_bool(node)) {
        lua_pushfstring(L, "%d", lilv_node_as_bool(node));
        goto finish;
    }

    lua_pushnil(L);
finish:
    return 1;
}

static int node_as_integer(lua_State *L) {
    node_t *n = node_check(L, 1);
    LilvNode *node = n->node;

    if (lilv_node_is_int(node)) {
        lua_pushinteger(L, lilv_node_as_int(node));
        goto finish;
    }

    lua_pushnil(L);
finish:
    return 1;
}

static int node_as_number(lua_State *L) {
    node_t *n = node_check(L, 1);
    LilvNode *node = n->node;

    if (lilv_node_is_float(node)) {
        lua_pushnumber(L, lilv_node_as_float(node));
        goto finish;
    }

    if (lilv_node_is_int(node)) {
        lua_pushnumber(L, lilv_node_as_int(node));
        goto finish;
    }

    lua_pushnil(L);
finish:
    return 1;
}

static int node_equals(lua_State *L) {
    // FIXME: this is broken
    node_t *a = node_check(L, 1);
    node_t *b = node_check(L, 2);
    lua_pushboolean(L, lilv_node_equals(a->node, b->node));
    return 1;
}

static int node_duplicate(lua_State *L) {
    node_t *n = node_check(L, 1);
    node_new(L, lilv_node_duplicate(n->node), true);
    return 1;
}




