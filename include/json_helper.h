#ifndef JSON_HELPER_H
#define JSON_HELPER_H
#include "toolbox/toolbox.h"
#include <json-c/json.h>
#define json_object_object_get_as_string(__jobj, __key)                                            \
  json_object_get_string(json_object_object_get(__jobj, __key))
#define json_object_object_get_as_double(__jobj, __key)                                            \
  json_object_get_double(json_object_object_get(__jobj, __key))
#define json_object_object_get_as_int(__jobj, __key)                                               \
  json_object_get_int(json_object_object_get(__jobj, __key))
#define json_object_array_get_idx_as_int(__jarr, __idx)                                            \
  json_object_get_int(json_object_array_get_idx(__jarr, __idx))
#define json_object_array_get_idx_as_string(__jarr, __idx)                                         \
  json_object_get_string(json_object_array_get_idx(__jarr, __idx))

typedef enum
{
  JSON_FIELD_TYPE_INT,
  JSON_FIELD_TYPE_STRING,
  JSON_FIELD_TYPE_STRING_ARRAY,
  JSON_FIELD_TYPE_INT_ARRAY,
  NUM_JSON_FIELD_TYPES,
} JsonFieldType;

typedef struct
{
  int         type;
  const char* name;
  size_t      offset;
} FieldMetaData;

#define JSON_FIELD(__T, __type, __name)                                                            \
  {                                                                                                \
    .type = __type, .name = #__name, .offset = offsetof(__T, __name)                             \
  }

#define JSON_FIELD_END                                                                             \
  {                                                                                                \
    .type = -1                                                                                     \
  }

typedef struct
{
  u32                  size;
  const FieldMetaData* fields;
} StructMetaData;

#define JSON_META_DATA(__T, __fields)                                                              \
  {                                                                                                \
    .size = sizeof(__T), .fields = __fields                                                        \
  }

json_object* load_json_from_file(const char* filename);
void*        json_to_struct(const StructMetaData* meta_data, struct json_object* json);
void         parse_struct(const FieldMetaData* fields, void* obj, struct json_object* json);
char*        struct_to_json(const StructMetaData* meta_data, const void* struct_ptr);

#endif // JSON_HELPER_H
