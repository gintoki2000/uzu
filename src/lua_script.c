#include "components.h"
#include "lauxlib.h"
#include "lua.h"
#include "luabinding.h"
#include "lualib.h"
#include "system/event_messaging_sys.h"

#define LUA_ON_ENTITY_DIED_FUNC_NAME "_on_entity_died"
#define LUA_ON_SCENE_LOAD "_on_scene_load"
#define LUA_ON_SCENE_UNLOAD "_on_scene_unload"
#define LUA_ON_LEVEL_LOAD "_on_level_load"
#define LUA_ON_LEVEL_UNLOAD "_on_level_unload"
#define LUA_ON_ITEM_PICKED_UP "_on_item_picked_up"

extern ecs_Registry* gRegistry;

typedef struct LuaScript
{
  lua_State* L;
} LuaScript;

static void on_entity_died(struct LuaScript* script, const EntityDiedMsg* msg);
static void on_scene_load(struct LuaScript* script, const void* msg);
static void on_scene_unload(struct LuaScript* script, const void* msg);
static void on_level_load(struct LuaScript* script, const LevelLoadedMsg* msg);
static void on_level_unload(struct LuaScript* script, const LevelLoadedMsg* msg);
static void on_item_pickuped(LuaScript* script, const ItemPickedUpMsg* msg);

static int lua_script_notify_finish(lua_State* L)
{
  const char* event  = luaL_checkstring(L, 1);
  const char* status = luaL_checkstring(L, 2);
  ems_broadcast(MSG_EVENT_FINISHED, &(EventFinishedMsg){ .event = event, .status = status });
  return 0;
}

static void lua_script_connect_signal(LuaScript* s, int signal, const char* luaFuncName, Func fn)
{
  lua_getglobal(s->L, luaFuncName);
  if (lua_isfunction(s->L, -1))
    ems_connect(signal, CALLBACK_1(s, fn));
  lua_pop(s->L, 1);
}

LuaScript* lua_script_create(const char* scriptFile)
{
  LuaScript* script = SDL_malloc(sizeof(LuaScript));
  lua_State* L = script->L = luaL_newstate();

  // load std lua libraries
  luaL_openlibs(L);

  // load scripting api
  lua_register(L, "notify_finish", lua_script_notify_finish);
  lua_openlib_entity(L);
  lua_openlib_inventory(L);
  lua_openlib_ui(L);
  lua_openlib_scene(L);

  // execute script
  luaL_dofile(L, scriptFile);

  lua_script_connect_signal(script, MSG_ENTITY_DIED, LUA_ON_ENTITY_DIED_FUNC_NAME, on_entity_died);
  lua_script_connect_signal(script, MSG_GAME_SCENE_LOADED, LUA_ON_SCENE_LOAD, on_scene_load);
  lua_script_connect_signal(script, MSG_GAME_SCENE_UNLOAD, LUA_ON_SCENE_UNLOAD, on_scene_unload);
  lua_script_connect_signal(script, MSG_LEVEL_LOADED, LUA_ON_LEVEL_LOAD, on_level_load);
  lua_script_connect_signal(script, MSG_LEVEL_UNLOADED, LUA_ON_LEVEL_UNLOAD, on_level_unload);
  lua_script_connect_signal(script, MSG_ITEM_PICKED_UP, LUA_ON_ITEM_PICKED_UP, on_item_pickuped);

  return script;
}

void lua_script_free(LuaScript* script)
{
  lua_close(script->L);
  SDL_free(script);
}

static void on_entity_died(LuaScript* script, const EntityDiedMsg* msg)
{
  lua_State* L = script->L;

  // push function to stack
  lua_getglobal(L, LUA_ON_ENTITY_DIED_FUNC_NAME);

  // set up function arguments
  // create a table to represent a message in lua
  lua_newtable(L);
  lua_pushstring(L, "entity");
  lua_pushentity(L, (lua_EntityHandle){ gRegistry, msg->entity });
  lua_settable(L, -3);

  // call lua function
  lua_call(L, 1, 0);
}

static void on_scene_load(LuaScript* script, SDL_UNUSED const void* msg)
{
  lua_State* L = script->L;

  lua_getglobal(L, LUA_ON_SCENE_LOAD);
  lua_call(L, 0, 0);
}

static void on_scene_unload(LuaScript* script, SDL_UNUSED const void* msg)
{
  lua_State* L = script->L;

  lua_getglobal(L, LUA_ON_SCENE_UNLOAD);
  lua_call(L, 0, 0);
}

static void on_level_load(LuaScript* script, const LevelLoadedMsg* msg)
{
  lua_State* L = script->L;

  lua_getglobal(L, LUA_ON_LEVEL_LOAD);

  lua_createtable(L, 0, 1);
  lua_pushstring(L, "level");
  lua_pushstring(L, msg->level_name);
  lua_settable(L, -3);

  lua_call(L, 1, 0);
}

static void on_level_unload(struct LuaScript* script, const LevelLoadedMsg* msg)
{
  lua_State* L = script->L;

  lua_getglobal(L, LUA_ON_LEVEL_UNLOAD);

  lua_createtable(L, 0, 1);
  lua_pushstring(L, "level");
  lua_pushstring(L, msg->level_name);
  lua_settable(L, -3);

  if (lua_pcall(L, 1, 0, 0) != LUA_OK)
  {
    printf("Fail to call lua function: %s\n", lua_tostring(L, -1));
  }
}

static void on_item_pickuped(LuaScript* script, const ItemPickedUpMsg* msg)
{
  lua_State* L = script->L;

  /*push function to stack*/
  lua_getglobal(L, LUA_ON_ITEM_PICKED_UP);

  /*set up arguments*/
  lua_newtable(L);
  lua_pushstring(L, "typeid");
  lua_pushinteger(L, msg->itemTypeId);
  lua_settable(L, -3);

  lua_pushstring(L, "item_name");
  lua_pushstring(L, gItemTypes[msg->itemTypeId].name);
  lua_settable(L, -3);

  // call function
  lua_call(L, 1, 0);
}

static void on_conversation_finished(LuaScript* self, const ConversationFinishedMsg* msg)
{
}
