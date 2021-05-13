#include "json_helper.h"
#include "read_all.h"
#include "global.h"

static void json_to_int(void* field, struct json_object*);
static void json_to_string(void* field, struct json_object*);
static void json_to_string_array(void* field, struct json_object*);

static void (*const _jsonToFieldDataFuncTabl[NUM_JSON_FIELD_TYPES])(void*, struct json_object*) = {
  [JSON_FIELD_TYPE_STRING]       = json_to_string,
  [JSON_FIELD_TYPE_STRING_ARRAY] = json_to_string_array,
  [JSON_FIELD_TYPE_INT]          = json_to_int,
};

void parse_struct(const FieldMetaData* fields, void* obj, struct json_object* json)
{
  void*               field;
  struct json_object* jsonField;

  for (int i = 0; fields[i].type != -1; ++i)
  {
    field      = (u8*)obj + fields[i].offset;
    jsonField = json_object_object_get(json, fields[i].name);
    _jsonToFieldDataFuncTabl[fields[i].type](field, jsonField);
  }
}

void* json_to_struct(const StructMetaData* metaData, struct json_object* json)
{
  void* obj;
  obj = SDL_malloc(metaData->size);
  parse_struct(metaData->fields, obj, json);
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

  array = ptr_array_new(SDL_free);
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
  json_object* jsonObj = NULL;

  if ((file = fopen(filename, "r")) != NULL)
  {
    if (readall(file, &filedata, &len) == READALL_OK)
    {
      jsonObj = json_tokener_parse(filedata);
      free(filedata);
    }
    fclose(file);
  }
  return jsonObj;
}
