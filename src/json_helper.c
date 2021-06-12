#include "json_helper.h"
#include "read_all.h"
#include "global.h"

static void json_to_int(void* fMem, struct json_object*);
static void json_to_string(void* fMem, struct json_object*);
static void json_to_string_array(void* fMem, struct json_object*);

static void (*const parsingFuncTbl[RFL_NUM_FIELD_TYPES])(void*, struct json_object*) = {
  [RFL_FIELD_TYPE_STRING]       = json_to_string,
  [RFL_FIELD_TYPE_STRING_ARRAY] = json_to_string_array,
  [RFL_FIELD_TYPE_INT]          = json_to_int,
};

void parse_struct(const rfl_Field* fields, void* obj, struct json_object* json)
{
  void*               fMem;
  struct json_object* jField;

  for (int i = 0; fields[i].type != -1; ++i)
  {
    fMem      = (u8*)obj + fields[i].offset;
    jField = json_object_object_get(json, fields[i].name);
    parsingFuncTbl[fields[i].type](fMem, jField);
  }
}

void* deserialize(const rfl_Struct* metaData, struct json_object* json)
{
  void* obj;
  obj = SDL_malloc(metaData->size);
  parse_struct(metaData->fields, obj, json);
  return obj;
}

static void json_to_int(void* fMem, struct json_object* json)
{
  *((int*)fMem) = json != NULL ? json_object_get_int(json) : 0;
}

static void json_to_string(void* fMem, struct json_object* json)
{
  *((char**)fMem) = json != NULL ? SDL_strdup(json_object_get_string(json)) : NULL;
}

static void json_to_string_array(void* fMem, struct json_object* json)
{
  PointerArray* array;
  const char* str;
  int         cnt;

  if (json == NULL)
  {
    *((PointerArray**)fMem) = NULL;
    return;
  }

  array = pointer_array_create(SDL_free);
  cnt   = json_object_array_length(json);
  for (int i = 0; i < cnt; ++i)
  {
    str = json_object_array_get_idx_as_string(json, i);
    pointer_array_add(array, SDL_strdup(str));
  }
  *((PointerArray**)fMem) = array;
}

json_object* load_json_from_file(const char* filename)
{
  FILE*        file;
  char*        filedata = NULL;
  size_t       len;
  json_object* jObj = NULL;

  if ((file = fopen(filename, "r")) != NULL)
  {
    if (readall(file, &filedata, &len) == READALL_OK)
    {
      jObj = json_tokener_parse(filedata);
      free(filedata);
    }
    fclose(file);
  }
  return jObj;
}
