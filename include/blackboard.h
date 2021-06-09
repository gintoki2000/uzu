#ifndef BLACKBOARD_H
#define BLACKBOARD_H
#include "ecs/ecs.h"
#include "toolbox/toolbox.h"
enum VariantType
{
  VARIANT_TYPE_UNDEFINED,
  VARIANT_TYPE_INTEGER,
  VARIANT_TYPE_BOOLEAN,
  VARIANT_TYPE_STRING,
  VARIANT_TYPE_ENTITY,
  VARIANT_TYPE_VEC2,
  VARIANT_TYPE_POINTER,
};

typedef struct _Variant
{
  enum VariantType type;
  union
  {
    int          integerValue;
    BOOL         booleanValue;
    const char*  stringValue;
    ecs_entity_t entityValue;
    void*        pointerValue;
    Vec2         vec2Value;
  };
} Variant;

#define VARIANT_INIT_INTEGER(value) { .type = VARIANT_TYPE_INTEGER, .integerValue = value }
#define VARIANT_INIT_STRING(value) { .type = VARIANT_TYPE_STRING, .stringValue = value }
#define VARIANT_INIT_BOOLEAN(value) { .type = VARIANT_TYPE_BOOLEAN, .booleanValue = value }
#define VARIANT_INIT_ENTITY(value) { .type = VARIANT_TYPE_ENTITY, .entityValue = value }
#define VARIANT_INIT_POINTER(value) { .type = VARIANT_TYPE_POINTER, .pointerValue = value }
#define VARIANT_INIT_VEC2(value) { .type = VARIANT_TYPE_VEC2, .vec2Value = value }
#define VARIANT_INIT_UNDEFINED { .type = VARIANT_TYPE_UNDEFINED }

#define VARIANT_INTEGER(value) (Variant){ .type = VARIANT_TYPE_INTEGER, .integerValue = value }
#define VARIANT_STRING(value) (Variant){ .type = VARIANT_TYPE_STRING, .stringValue = value }
#define VARIANT_BOOLEAN(value) (Variant){ .type = VARIANT_TYPE_BOOLEAN, .booleanValue = value }
#define VARIANT_ENTITY(value) (Variant){ .type = VARIANT_TYPE_ENTITY, .entityValue = value }
#define VARIANT_POINTER(value) (Variant){ .type = VARIANT_TYPE_POINTER, .pointerValue = value }
#define VARIANT_VEC2(value) (Variant){ .type = VARIANT_TYPE_VEC2, .vec2Value = value }
#define VARIANT_UNDEFINED (Variant){ .type = VARIANT_TYPE_UNDEFINED }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
int variant_get_as_integer(const Variant* v);
BOOL variant_get_as_boolean(const Variant* v);
const char* variant_get_as_string(const Variant* v);
ecs_entity_t variant_get_as_entity(const Variant* v);
Vec2 variant_get_as_vec2(const Variant* v);
void* variant_get_as_pointer(const Variant* v);

INLINE void variant_set_as_integer(Variant* v, int value)
{
  v->type = VARIANT_TYPE_INTEGER;
  v->integerValue = value;
}

INLINE void variant_set_as_boolean(Variant* v, BOOL value)
{
  v->type = VARIANT_TYPE_BOOLEAN;
  v->booleanValue = value;
}

INLINE void variant_set_as_string(Variant* v, const char* value)
{
  v->type = VARIANT_TYPE_STRING;
  v->stringValue = value;
}

INLINE void variant_set_as_entity(Variant* v, ecs_entity_t value)
{
  v->type = VARIANT_TYPE_ENTITY;
  v->entityValue = value;
}
INLINE void variant_set_as_vec2(Variant* v, Vec2 value)
{
  v->type = VARIANT_TYPE_VEC2;
  v->vec2Value = value;
}

INLINE void variant_set_as_pointer(Variant* v, void* value)
{
  v->type = VARIANT_TYPE_POINTER;
  v->pointerValue = value;
}
#pragma GCC diagnostic pop


typedef struct _Blackboard
{
  Variant* entries;
  u32 numEntries;
} Blackboard;


Blackboard* blackboard_create(u32 numEntries);
void blackboard_free(Blackboard* bb);

void blackboard_clear(Blackboard* blackboard);
Variant* blackboard_get_entry(Blackboard* blackboard, u32 id);
void blackboard_unset_entry(Blackboard* blackboard, u32 entryid);
void blackboard_set_entry_as_integer(Blackboard* blackboard, u32 entryid, int value);
void blackboard_set_entry_as_boolean(Blackboard* blackboard, u32 entryid, BOOL value);
void blackboard_set_entry_as_string(Blackboard* blackboard, u32 entryid, const char* value);
void blackboard_set_entry_as_entity(Blackboard* blackboard, u32 entryid, ecs_entity_t value);
void blackboard_set_entry_as_pointer(Blackboard* blackboard, u32 entryid, void* value);
void blackboard_set_entry_as_vec2(Blackboard* blackboard, u32 entryid, Vec2 value);

BOOL blackboard_entry_is_set(Blackboard* blackboard, u32 entryid);
BOOL blackboard_get_entry_as_integer(Blackboard* blackboard, u32 entryid, int* outValue);
BOOL blackboard_get_entry_as_boolean(Blackboard* blackboard, u32 entryid, BOOL* outValue);
BOOL blackboard_get_entry_as_string(Blackboard* blackboard, u32 entryid, const char** outValue);
BOOL blackboard_get_entry_as_entity(Blackboard* blackboard, u32 entryid, ecs_entity_t* outValue);
BOOL blackboard_get_entry_as_pointer(Blackboard* blackboard, u32 entryid, void** outValue);
BOOL blackboard_get_entry_as_vec2(Blackboard* blackboard, u32 entryid, Vec2* outValue);

#endif // BLACKBOARD_H
