#include "lilv/lilv.h"

#include "lua_lilv.h"
#include "world.h"

//
// prototypes
//

static int world_new(lua_State *L);
static int world_free(lua_State *L);
static int world_load_all(lua_State *L);

//
// world object methods
//

static char *world_classname = "World";
static luaL_Reg world_methods[] = {
    {"__gc",      world_free},
    {"load_all",  world_load_all},
    {NULL,        NULL}
};

//
// module functions
//

static luaL_Reg module_functions[] = {
    {"new",       world_new},
    {NULL,        NULL}
};

int world_open(lua_State *L) {
    // module name
    lua_pushstring(L, world_classname);

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

    //lua_pop(L, 1);                        // core

    return 0;
}

//
// implementation
//

static LilvWorld **world_check(lua_State *L) {
    void *ud = luaL_checkudata(L, 1, world_classname);
    luaL_argcheck(L, ud != NULL, 1, "'world' expected");
    return (LilvWorld **)ud;
}

static int world_new(lua_State *L) {
    LilvWorld **w = (LilvWorld **)lua_newuserdata(L, sizeof(LilvWorld *));
    *w = lilv_world_new();
    luaL_getmetatable(L, world_classname);
    lua_setmetatable(L, -2);
    return 1;
}

static int world_free(lua_State *L) {
    LilvWorld **w = world_check(L);
    if (*w != NULL) {
        lilv_world_free(*w);
        *w = NULL;
    }
    return 0;
}

static int world_load_all(lua_State *L) {
    LilvWorld **w = world_check(L);
    lilv_world_load_all(*w);
    return 0;
}
