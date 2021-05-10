#include "blackboard.h"
int variant_get_as_integer(const Variant* v)
{
  ASSERT(v->type == VARIANT_TYPE_INTEGER);
  return v->integer_value;
}

BOOL variant_get_as_boolean(const Variant* v)
{
  ASSERT(v->type == VARIANT_TYPE_BOOLEAN);
  return v->boolean_value;
}

const char* variant_get_as_string(const Variant* v)
{
  ASSERT(v->type == VARIANT_TYPE_STRING);
  return v->string_value;
}

ecs_entity_t variant_get_as_entity(const Variant* v)
{
  ASSERT(v->type == VARIANT_TYPE_ENTITY);
  return v->entity_value;
}

Vec2 variant_get_as_vec2(const Variant* v)
{
  ASSERT(v->type == VARIANT_TYPE_VEC2);
  return v->vec2_value;
}

void* variant_get_as_pointer(const Variant* v)
{
  ASSERT(v->type == VARIANT_TYPE_POINTER);
  return v->pointer_value;
}

void blackboard_init(Blackboard* self, int num_entries)
{
  self->entries     = SDL_malloc(sizeof(Variant) * num_entries);
  self->num_entries = num_entries;
  for (int i = 0; i < num_entries; ++i)
    self->entries[i] = VARIANT_UNDEFINED;
}

void blackboard_fini(Blackboard* self)
{
  SDL_free(self->entries);
  self->entries     = NULL;
  self->num_entries = 0;
}

void blackboard_clear(Blackboard* self)
{
  for (int i = 0; i < self->num_entries; ++i)
    self->entries[i] = VARIANT_UNDEFINED;
}
Variant* blackboard_get_entry(Blackboard* self, int id)
{
  ASSERT(id < self->num_entries);
  return &self->entries[id];
}
void blackboard_unset_entry(Blackboard* self, int entryid)
{
  ASSERT(entryid < self->num_entries);
  self->entries[entryid] = VARIANT_UNDEFINED;
}
void blackboard_set_entry_as_integer(Blackboard* self, int entryid, int value)
{
  ASSERT(entryid < self->num_entries);
  self->entries[entryid] = VARIANT_INTEGER(value);
}
void blackboard_set_entry_as_boolean(Blackboard* self, int entryid, BOOL value)
{
  ASSERT(entryid < self->num_entries);
  self->entries[entryid] = VARIANT_BOOLEAN(value);
}
void blackboard_set_entry_as_string(Blackboard* self, int entryid, const char* value)
{
  ASSERT(entryid < self->num_entries);
  self->entries[entryid] = VARIANT_STRING(value);
}
void blackboard_set_entry_as_entity(Blackboard* self, int entryid, ecs_entity_t value)
{
  ASSERT(entryid < self->num_entries);
  self->entries[entryid] = VARIANT_ENTITY(value);
}
void blackboard_set_entry_as_pointer(Blackboard* self, int entryid, void* value)
{
  ASSERT(entryid < self->num_entries);
  self->entries[entryid] = VARIANT_POINTER(value);
}
void blackboard_set_entry_as_vec2(Blackboard* self, int entryid, Vec2 value)
{
  ASSERT(entryid < self->num_entries);
  self->entries[entryid] = VARIANT_VEC2(value);
}

BOOL blackboard_entry_is_set(Blackboard* self, int entryid)
{
  ASSERT(entryid >= 0 && entryid < self->num_entries);
  return self->entries[entryid].type != VARIANT_TYPE_UNDEFINED;
}

BOOL blackboard_get_entry_as_integer(Blackboard* self, int entryid, int* out_value)
{
  ASSERT(entryid < self->num_entries);
  if (self->entries[entryid].type == VARIANT_TYPE_UNDEFINED)
    return FALSE;
  *out_value = variant_get_as_integer(&self->entries[entryid]);
  return TRUE;
}

BOOL blackboard_get_entry_as_boolean(Blackboard* self, int entryid, BOOL* out_value)
{
  ASSERT(entryid < self->num_entries);
  if (self->entries[entryid].type == VARIANT_TYPE_UNDEFINED)
    return FALSE;
  *out_value = variant_get_as_boolean(&self->entries[entryid]);
  return TRUE;
}

BOOL blackboard_get_entry_as_string(Blackboard* self, int entryid, const char** out_value)
{
  ASSERT(entryid < self->num_entries);
  if (self->entries[entryid].type == VARIANT_TYPE_UNDEFINED)
    return FALSE;
  *out_value = variant_get_as_string(&self->entries[entryid]);
  return TRUE;
}
BOOL blackboard_get_entry_as_entity(Blackboard* self, int entryid, ecs_entity_t* out_value)
{
  ASSERT(entryid < self->num_entries);
  if (self->entries[entryid].type == VARIANT_TYPE_UNDEFINED)
    return FALSE;
  *out_value = variant_get_as_entity(&self->entries[entryid]);
  return TRUE;
}

BOOL blackboard_get_entry_as_pointer(Blackboard* self, int entryid, void** out_value)
{
  ASSERT(entryid < self->num_entries);
  if (self->entries[entryid].type == VARIANT_TYPE_UNDEFINED)
    return FALSE;
  *out_value = variant_get_as_pointer(&self->entries[entryid]);
  return TRUE;
}

BOOL blackboard_get_entry_as_vec2(Blackboard* self, int entryid, Vec2* out_value)
{
  ASSERT(entryid < self->num_entries);
  if (self->entries[entryid].type == VARIANT_TYPE_UNDEFINED)
    return FALSE;
  *out_value = variant_get_as_vec2(&self->entries[entryid]);
  return TRUE;
}
