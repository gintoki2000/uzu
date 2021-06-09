#include "blackboard.h"
int variant_get_as_integer(const Variant* v)
{
  ASSERT(v->type == VARIANT_TYPE_INTEGER);
  return v->integerValue;
}

BOOL variant_get_as_boolean(const Variant* v)
{
  ASSERT(v->type == VARIANT_TYPE_BOOLEAN);
  return v->booleanValue;
}

const char* variant_get_as_string(const Variant* v)
{
  ASSERT(v->type == VARIANT_TYPE_STRING);
  return v->stringValue;
}

ecs_entity_t variant_get_as_entity(const Variant* v)
{
  ASSERT(v->type == VARIANT_TYPE_ENTITY);
  return v->entityValue;
}

Vec2 variant_get_as_vec2(const Variant* v)
{
  ASSERT(v->type == VARIANT_TYPE_VEC2);
  return v->vec2Value;
}

void* variant_get_as_pointer(const Variant* v)
{
  ASSERT(v->type == VARIANT_TYPE_POINTER);
  return v->pointerValue;
}

void blackboard_free(Blackboard* self)
{
  if (self)
    SDL_free(self->entries);
  SDL_free(self);
}

Blackboard* blackboard_create(u32 numEntries)
{
  Blackboard* self = SDL_malloc(sizeof(Blackboard));
  self->entries    = SDL_malloc(sizeof(Variant) * numEntries);
  self->numEntries = numEntries;
  for (u32 i = 0; i < numEntries; ++i)
    self->entries[i] = VARIANT_UNDEFINED;
  return self;
}

void blackboard_clear(Blackboard* self)
{
  for (u32 i = 0; i < self->numEntries; ++i)
    self->entries[i] = VARIANT_UNDEFINED;
}
Variant* blackboard_get_entry(Blackboard* self, u32 id)
{
  ASSERT(id < self->numEntries);
  return &self->entries[id];
}
void blackboard_unset_entry(Blackboard* self, u32 entryid)
{
  ASSERT(entryid < self->numEntries);
  self->entries[entryid] = VARIANT_UNDEFINED;
}
void blackboard_set_entry_as_integer(Blackboard* self, u32 entryid, int value)
{
  ASSERT(entryid < self->numEntries);
  self->entries[entryid] = VARIANT_INTEGER(value);
}
void blackboard_set_entry_as_boolean(Blackboard* self, u32 entryid, BOOL value)
{
  ASSERT(entryid < self->numEntries);
  self->entries[entryid] = VARIANT_BOOLEAN(value);
}
void blackboard_set_entry_as_string(Blackboard* self, u32 entryid, const char* value)
{
  ASSERT(entryid < self->numEntries);
  self->entries[entryid] = VARIANT_STRING(value);
}
void blackboard_set_entry_as_entity(Blackboard* self, u32 entryid, ecs_entity_t value)
{
  ASSERT(entryid < self->numEntries);
  self->entries[entryid] = VARIANT_ENTITY(value);
}
void blackboard_set_entry_as_pointer(Blackboard* self, u32 entryid, void* value)
{
  ASSERT(entryid < self->numEntries);
  self->entries[entryid] = VARIANT_POINTER(value);
}
void blackboard_set_entry_as_vec2(Blackboard* self, u32 entryid, Vec2 value)
{
  ASSERT(entryid < self->numEntries);
  self->entries[entryid] = VARIANT_VEC2(value);
}

BOOL blackboard_entry_is_set(Blackboard* self, u32 entryid)
{
  ASSERT(entryid < self->numEntries);
  return self->entries[entryid].type != VARIANT_TYPE_UNDEFINED;
}

BOOL blackboard_get_entry_as_integer(Blackboard* self, u32 entryid, int* outValue)
{
  ASSERT(entryid < self->numEntries);
  if (self->entries[entryid].type == VARIANT_TYPE_UNDEFINED)
    return FALSE;
  *outValue = variant_get_as_integer(&self->entries[entryid]);
  return TRUE;
}

BOOL blackboard_get_entry_as_boolean(Blackboard* self, u32 entryid, BOOL* outValue)
{
  ASSERT(entryid < self->numEntries);
  if (self->entries[entryid].type == VARIANT_TYPE_UNDEFINED)
    return FALSE;
  *outValue = variant_get_as_boolean(&self->entries[entryid]);
  return TRUE;
}

BOOL blackboard_get_entry_as_string(Blackboard* self, u32 entryid, const char** outValue)
{
  ASSERT(entryid < self->numEntries);
  if (self->entries[entryid].type == VARIANT_TYPE_UNDEFINED)
    return FALSE;
  *outValue = variant_get_as_string(&self->entries[entryid]);
  return TRUE;
}
BOOL blackboard_get_entry_as_entity(Blackboard* self, u32 entryid, ecs_entity_t* outValue)
{
  ASSERT(entryid < self->numEntries);
  if (self->entries[entryid].type == VARIANT_TYPE_UNDEFINED)
    return FALSE;
  *outValue = variant_get_as_entity(&self->entries[entryid]);
  return TRUE;
}

BOOL blackboard_get_entry_as_pointer(Blackboard* self, u32 entryid, void** outValue)
{
  ASSERT(entryid < self->numEntries);
  if (self->entries[entryid].type == VARIANT_TYPE_UNDEFINED)
    return FALSE;
  *outValue = variant_get_as_pointer(&self->entries[entryid]);
  return TRUE;
}

BOOL blackboard_get_entry_as_vec2(Blackboard* self, u32 entryid, Vec2* outValue)
{
  ASSERT(entryid < self->numEntries);
  if (self->entries[entryid].type == VARIANT_TYPE_UNDEFINED)
    return FALSE;
  *outValue = variant_get_as_vec2(&self->entries[entryid]);
  return TRUE;
}
