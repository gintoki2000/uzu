// ngotrung Tue 08 Jun 2021 06:20:12 PM +07
#ifndef LUABINDING_H
#define LUABINDING_H
#include "ecs/ecs.h"
#include "lua.h"
#include "rfl.h"

typedef struct lua_EntityHandle
{
  ecs_Registry* registry;
  ecs_entity_t  entity;
} lua_EntityHandle;

int              lua_openlib_entity(lua_State* L);
void             lua_pushentity(lua_State* L, lua_EntityHandle handle);
lua_EntityHandle lua_checkentity(lua_State* L, int index);

int lua_openlib_inventory(lua_State* L);
int lua_openlib_ui(lua_State* L);
int lua_openlib_scene(lua_State* L);

int lua_pushstruct(lua_State* L, const rfl_Struct* md, const void* s)
#endif // LUABINDING_H
