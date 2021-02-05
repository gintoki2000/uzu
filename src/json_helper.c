#include "json_helper.h"
#include "read_all.h"
#include "global.h"

static void json_to_int(void* field, struct json_object*);
static void json_to_string(void* field, struct json_object*);
static void json_to_string_array(void* field, struct json_object*);

void (*JSON_TO_FIELD_DATA_FN_TBL[NUM_JSON_FIELD_TYPES])(void*, struct json_object*) = {
  [JSON_FIELD_TYPE_STRING]       = json_to_string,
  [JSON_FIELD_TYPE_STRING_ARRAY] = json_to_string_array,
  [JSON_FIELD_TYPE_INT]          = json_to_int,
};

void parse_struct(const FieldMetaData* fields, void* obj, struct json_object* json)
{
  void*               obj_field;
  struct json_object* field_json_obj;

  for (int i = 0; fields[i].type != -1; ++i)
  {
    obj_field      = (u8*)obj + fields[i].offset;
    field_json_obj = json_object_object_get(json, fields[i].name);
    JSON_TO_FIELD_DATA_FN_TBL[fields[i].type](obj_field, field_json_obj);
  }
}

void* json_to_struct(const StructMetaData* meta_data, struct json_object* json)
{
  void* obj;
  obj = malloc(meta_data->size);
  parse_struct(meta_data->fields, obj, json);
  return obj;
}

static void json_to_int(void* field, struct json_object* json)
{
  *((int*)field) = json != NULL ? json_object_get_int(json) : 0;
}

static void json_to_string(void* field, struct json_object* json)
{
  *((char**)field) = json != NULL ? SDL_strdup(json_object_get_string(json)) : NULL;
}

static void json_to_string_array(void* field, struct json_object* json)
{
  PtrArray*   array;
  const char* str;
  int         cnt;

  if (json == NULL)
  {
    *((PtrArray**)field) = NULL;
    return;
  }

  array = ptr_array_new(free);
  cnt   = json_object_array_length(json);
  for (int i = 0; i < cnt; ++i)
  {
    str = json_object_array_get_idx_as_string(json, i);
    ptr_array_add(array, SDL_strdup(str));
  }
  *((PtrArray**)field) = array;
}

json_object* load_json_from_file(const char* filename)
{
  FILE*        file;
  char*        filedata = NULL;
  size_t       len;
  json_object* json_obj = NULL;

  if ((file = fopen(filename, "r")) != NULL)
  {
    if (readall(file, &filedata, &len) == READALL_OK)
    {
      json_obj = json_tokener_parse(filedata);
      free(filedata);
    }
    fclose(file);
  }
  return json_obj;
}
