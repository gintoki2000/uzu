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
    int          integer_value;
    BOOL         boolean_value;
    const char*  string_value;
    ecs_entity_t entity_value;
    void*        pointer_value;
    Vec2         vec2_value;
  };
} Variant;

#define VARIANT_INIT_INTEGER(value) { .type = VARIANT_TYPE_INTEGER, .integer_value = value }
#define VARIANT_INIT_STRING(value) { .type = VARIANT_TYPE_STRING, .string_value = value }
#define VARIANT_INIT_BOOLEAN(value) { .type = VARIANT_TYPE_BOOLEAN, .boolean_value = value }
#define VARIANT_INIT_ENTITY(value) { .type = VARIANT_TYPE_ENTITY, .entity_value = value }
#define VARIANT_INIT_POINTER(value) { .type = VARIANT_TYPE_POINTER, .pointer_value = value }
#define VARIANT_INIT_VEC2(value) { .type = VARIANT_TYPE_VEC2, .vec2_value = value }
#define VARIANT_INIT_UNDEFINED { .type = VARIANT_TYPE_UNDEFINED }

#define VARIANT_INTEGER(value) (Variant){ .type = VARIANT_TYPE_INTEGER, .integer_value = value }
#define VARIANT_STRING(value) (Variant){ .type = VARIANT_TYPE_STRING, .string_value = value }
#define VARIANT_BOOLEAN(value) (Variant){ .type = VARIANT_TYPE_BOOLEAN, .boolean_value = value }
#define VARIANT_ENTITY(value) (Variant){ .type = VARIANT_TYPE_ENTITY, .entity_value = value }
#define VARIANT_POINTER(value) (Variant){ .type = VARIANT_TYPE_POINTER, .pointer_value = value }
#define VARIANT_VEC2(value) (Variant){ .type = VARIANT_TYPE_VEC2, .vec2_value = value }
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
  v->integer_value = value;
}

INLINE void variant_set_as_boolean(Variant* v, BOOL value)
{
  v->type = VARIANT_TYPE_BOOLEAN;
  v->boolean_value = value;
}

INLINE void variant_set_as_string(Variant* v, const char* value)
{
  v->type = VARIANT_TYPE_STRING;
  v->string_value = value;
}

INLINE void variant_set_as_entity(Variant* v, ecs_entity_t value)
{
  v->type = VARIANT_TYPE_ENTITY;
  v->entity_value = value;
}
INLINE void variant_set_as_vec2(Variant* v, Vec2 value)
{
  v->type = VARIANT_TYPE_VEC2;
  v->vec2_value = value;
}

INLINE void variant_set_as_pointer(Variant* v, void* value)
{
  v->type = VARIANT_TYPE_POINTER;
  v->pointer_value = value;
}
#pragma GCC diagnostic pop


typedef struct _Blackboard
{
  Variant* entries;
  int      num_entries;
} Blackboard;

void blackboard_init(Blackboard* blackboard, int num_entries);
void blackboard_fini(Blackboard* blackboard);
void blackboard_clear(Blackboard* blackboard);
Variant* blackboard_get_entry(Blackboard* blackboard, int id);
void blackboard_unset_entry(Blackboard* blackboard, int entryid);
void blackboard_set_entry_as_integer(Blackboard* blackboard, int entryid, int value);
void blackboard_set_entry_as_boolean(Blackboard* blackboard, int entryid, BOOL value);
void blackboard_set_entry_as_string(Blackboard* blackboard, int entryid, const char* value);
void blackboard_set_entry_as_entity(Blackboard* blackboard, int entryid, ecs_entity_t value);
void blackboard_set_entry_as_pointer(Blackboard* blackboard, int entryid, void* value);
void blackboard_set_entry_as_vec2(Blackboard* blackboard, int entryid, Vec2 value);

BOOL blackboard_entry_is_set(Blackboard* blackboard, int entryid);
BOOL blackboard_get_entry_as_integer(Blackboard* blackboard, int entryid, int* out_value);
BOOL blackboard_get_entry_as_boolean(Blackboard* blackboard, int entryid, BOOL* out_value);
BOOL blackboard_get_entry_as_string(Blackboard* blackboard, int entryid, const char** out_value);
BOOL blackboard_get_entry_as_entity(Blackboard* blackboard, int entryid, ecs_entity_t* out_value);
BOOL blackboard_get_entry_as_pointer(Blackboard* blackboard, int entryid, void** out_value);
BOOL blackboard_get_entry_as_vec2(Blackboard* blackboard, int entryid, Vec2* out_value);

#endif // BLACKBOARD_H
