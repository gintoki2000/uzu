// ngotrung Tue 08 Jun 2021 06:19:01 PM +07
//

#include "luabinding.h"
#include "components.h"
#include "entity_factory.h"
#include "inventory.h"
#include "lauxlib.h"
#include "resources.h"
#include "ui_msgbox.h"

void lua_pushentity(lua_State* L, lua_EntityHandle handle)
{
  lua_EntityHandle* luaRepresent = lua_newuserdata(L, sizeof(lua_EntityHandle));
  lua_getglobal(L, "Entity");
  lua_setmetatable(L, -2);

  luaRepresent->entity   = handle.entity;
  luaRepresent->registry = handle.registry;
}

lua_EntityHandle lua_checkentity(lua_State* L, int index)
{
  lua_EntityHandle* handle = lua_touserdata(L, index);

  lua_getmetatable(L, index);
  lua_getglobal(L, "Entity");
  luaL_argcheck(L, lua_compare(L, -1, -2, LUA_OPEQ), index, "expect \'Entity\'");
  lua_pop(L, 2);

  return *handle;
}

static int lua_entity_ismon(lua_State* L)
{
  lua_EntityHandle handle = lua_checkentity(L, 1);
  lua_pushboolean(L, ecs_has(handle.registry, handle.entity, ENEMY_TAG));
  return 1;
}

static int lua_entity_getname(lua_State* L)
{
  lua_EntityHandle handle        = lua_checkentity(L, 1);
  Name*            nameComponent = ecs_get(handle.registry, handle.entity, NAME);
  if (nameComponent != NULL)
    lua_pushstring(L, nameComponent->value);
  else
    lua_pushnil(L);
  return 1;
}

static int lua_entity_getpos(lua_State* L)
{
  lua_EntityHandle handle = lua_checkentity(L, 1);
  Transform*       t      = ecs_get(handle.registry, handle.entity, TRANSFORM);
  lua_pushnumber(L, t->position.x);
  lua_pushnumber(L, t->position.y);

  return 2;
}

static int lua_entity_getdialogue(lua_State* L)
{
  lua_EntityHandle handle            = lua_checkentity(L, 1);
  Dialogue*        dialogueComponent = ecs_get(handle.registry, handle.entity, DIALOGUE);
  lua_pushstring(L, dialogueComponent->conversation->name);
  return 1;
}

static int lua_entity_setdialogue(lua_State* L)
{
  lua_EntityHandle    handle       = lua_checkentity(L, 1);
  const char*         name         = luaL_checkstring(L, 2);
  const Conversation* conversation = get_conversation(name);
  if (conversation != NULL)
    ecs_set(handle.registry, handle.entity, DIALOGUE, &(Dialogue){ conversation });
  else
    printf("no conversation name %s", name);
  return 0;
}

static int lua_entity_setsupply(lua_State* L)
{
  return 0;
}

static int lua_entity_setwellcome(lua_State* L)
{
  return 0;
}

static int lua_entity_setgoodbye(lua_State* L)
{
  return 0;
}

static const luaL_Reg _libentity[] = {
  { "get_name", lua_entity_getname },         { "is_monster", lua_entity_ismon },
  { "get_position", lua_entity_getpos },      { "set_dialogue", lua_entity_setdialogue },
  { "get_dialogue", lua_entity_getdialogue }, { NULL, NULL },
};

int lua_openlib_entity(lua_State* L)
{
  luaL_newlib(L, _libentity);
  lua_setglobal(L, "Entity");
  luaL_dostring(L, "Entity.__index = Entity");
  return 1;
}

//*********************************************//

struct _ItemId
{
  const char* luaname;
  int         cid;
};

static int lua_invadd(lua_State* L)
{
  ItemTypeId itemTypeId = luaL_checkinteger(L, 1);
  int        quality    = luaL_checkinteger(L, 2);
  inv_add_item(itemTypeId, quality);
  return 0;
}

static int lua_pushitem(lua_State* L, Item* item)
{
  if (item == NULL)
    lua_pushnil(L);
  else
  {
    lua_pushlightuserdata(L, item);
    lua_getglobal(L, "Item");
    lua_setmetatable(L, -2);
  }
  return 1;
}

static Item* lua_checkitem(lua_State* L, int index)
{
  Item* item = lua_touserdata(L, index);
  lua_getglobal(L, "Item");
  lua_getmetatable(L, index);
  luaL_argcheck(L, lua_compare(L, -1, -2, LUA_OPEQ), 1, "expect \'Item\'");
  lua_pop(L, 2);
  return item;
}

static int lua_invget(lua_State* L)
{
  ItemTypeId itemTypeId = luaL_checkinteger(L, 1);

  Item* item = inv_get_item(itemTypeId);
  lua_pushitem(L, item);
  return 1;
}

static int lua_item_getquality(lua_State* L)
{
  Item* item = lua_checkitem(L, 1);
  lua_pushinteger(L, item->quality);
  return 1;
}

static int lua_item_setquality(lua_State* L)
{
  Item* item    = lua_checkitem(L, 1);
  item->quality = luaL_checkinteger(L, 2);
  return 0;
}

static int lua_item_getname(lua_State* L)
{
  Item* item = lua_checkitem(L, 1);
  lua_pushstring(L, gItemTypes[item->type].name);
  return 1;
}

static int lua_item_drop(lua_State* L)
{
  Item* item = lua_checkitem(L, 1);
  int   n    = luaL_checkinteger(L, 2);

  item->quality -= max(0, min(n, item->quality));
  return 0;
}

static const struct _ItemId _itemIds[] = {
  { "RED_FLASK", ITEM_TYPE_RED_FLASK },
  { "BIG_RED_FLASK", ITEM_TYPE_BIG_RED_FLASK },
  { "BLUE_FLASK", ITEM_TYPE_BLUE_FLASK },
};

static const luaL_Reg _libinv[] = {
  { "get_item", lua_invget },
  { "add_item", lua_invadd },
};

static const luaL_Reg _libitem[] = {
  { "get_quality", lua_item_getquality },
  { "set_quality", lua_item_setquality },
  { "get_name", lua_item_getname },
  { "drop", lua_item_drop },
};

int lua_openlib_inventory(lua_State* L)
{
  int n = sizeof(_itemIds) / sizeof(struct _ItemId);
  luaL_newlib(L, _libitem);
  for (int i = 0; i < n; ++i)
  {
    lua_pushstring(L, _itemIds[i].luaname);
    lua_pushinteger(L, _itemIds[i].cid);
    lua_settable(L, -3);
  }
  lua_setglobal(L, "Item");
  luaL_dostring(L, "Item.__index = Item");

  luaL_newlib(L, _libinv);
  lua_setglobal(L, "inventory");
  return 1;
}

/* scene lib*/
extern ecs_Registry* gRegistry;

static int lua_scene_newbrian(lua_State* L)
{

  Vec2        position;
  const char* dialogue;

  position.x = luaL_checkinteger(L, 1);
  position.y = luaL_checknumber(L, 2);
  dialogue   = luaL_checkstring(L, 3);

  ecs_entity_t brian = make_npc_brian(gRegistry, position, dialogue);

  lua_pushentity(L, (lua_EntityHandle){ .registry = gRegistry, .entity = brian });

  return 1;
}

static int lua_scene_newchort(lua_State* L)
{
  Vec2 position;

  position.x = luaL_checkinteger(L, 1);
  position.y = luaL_checkinteger(L, 2);

  ecs_entity_t chort = make_chort(gRegistry, position);

  lua_pushentity(L, (lua_EntityHandle){ .registry = gRegistry, .entity = chort });
  return 1;
}

static const luaL_Reg _libscene[] = {
  { "new_brian", lua_scene_newbrian },
  { "new_chort", lua_scene_newchort },
  { NULL, NULL },
};

int lua_openlib_scene(lua_State* L)
{
  luaL_newlib(L, _libscene);
  lua_setglobal(L, "scene");
  return 1;
}

/* ui lib*/

static int lua_ui_showmsgbox(lua_State* L)
{
  const char* msg = lua_tostring(L, 1);
  ui_msgbox_display(msg);
  return 0;
}

static const luaL_Reg _libui[] = {
  { "show_msgbox", lua_ui_showmsgbox },
  { NULL, NULL },
};

int lua_openlib_ui(lua_State* L)
{
  luaL_newlib(L, _libui);
  lua_setglobal(L, "ui");

  return 1;
}
