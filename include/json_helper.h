#ifndef JSON_HELPER_H
#define JSON_HELPER_H
#include "toolbox/toolbox.h"
#include <json-c/json.h>

#include "rfl.h"
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

json_object* load_json_from_file(const char* filename);
void*        deserialize(const rfl_Struct* metadata, struct json_object* json);
char*        serialize(const rfl_Struct* metadata, const void* obj);

#endif // JSON_HELPER_H
