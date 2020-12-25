#include "map_loader.h"
#include "entity_factory.h"
#include "map.h"
#include "read_all.h"
#include <json-c/json.h>
#include <json-c/json_visit.h>

#define json_object_object_get_as_string(__jobj, __key)                                            \
  json_object_get_string(json_object_object_get(__jobj, __key))
#define json_object_object_get_as_double(__jobj, __key)                                            \
  json_object_get_double(json_object_object_get(__jobj, __key))
#define json_object_object_get_as_int(__jobj, __key)                                               \
  json_object_get_int(json_object_object_get(__jobj, __key))
#define json_object_array_get_idx_as_int(__jarr, __idx)                                            \
  json_object_get_int(json_object_array_get_idx(__jarr, __idx))

static int parse_tilelayer(const json_object* tilelayer_json_obj)
{
  int*               data;
  int                datalen;
  const json_object* data_json_obj;
  const char*        name;
  int                to_layer;

  data_json_obj = json_object_object_get(tilelayer_json_obj, "data");
  name          = json_object_object_get_as_string(tilelayer_json_obj, "name");
  datalen       = json_object_array_length(data_json_obj);

  data = malloc(datalen * sizeof(int));
  for (int i = 0; i < datalen; ++i)
  {
    data[i] = json_object_array_get_idx_as_int(data_json_obj, i);
  }
  printf("\n");

  if (strcmp(name, "floor") == 0)
  {
    to_layer = MAP_LAYER_FLOOR;
  }
  else if (strcmp(name, "front-wall") == 0)
  {
    to_layer = MAP_LAYER_FRONT;
  }
  else if (strcmp(name, "back-wall") == 0)
  {
    to_layer = MAP_LAYER_WALL;
  }
  else
  {
    to_layer = -1;
  }

  if (to_layer != -1)
  {
    map_set_data(to_layer, data, datalen);
  }
  free(data);

  return 0;
}

static int parse_objectgroup(const json_object* object_group_json_obj, Ecs* ecs)
{
  const json_object* objects_json_obj;
  const json_object* props_json_obj;
  int                objcnt;
  const char*        objtype;
  const json_object* obj_json_obj;
  Vec2               pos;
  Vec2               size;
  objects_json_obj = json_object_object_get(object_group_json_obj, "objects");
  objcnt           = json_object_array_length(objects_json_obj);
  for (int i = 0; i < objcnt; ++i)
  {
    obj_json_obj = json_object_array_get_idx(objects_json_obj, i);
    objtype      = json_object_get_string(json_object_object_get(obj_json_obj, "type"));
    if (strcmp(objtype, "Chort") == 0)
    {
      pos.x = json_object_get_double(json_object_object_get(obj_json_obj, "x"));
      pos.y = json_object_get_double(json_object_object_get(obj_json_obj, "y"));
      make_chort(ecs, pos);
    }
    else if (strcmp(objtype, "Ladder") == 0)
    {
      const char* name;
      const char* next_map;
      const char* link_with;
      pos.x  = json_object_object_get_as_double(obj_json_obj, "x");
      pos.y  = json_object_object_get_as_double(obj_json_obj, "y");
      size.x = json_object_object_get_as_double(obj_json_obj, "width");
      size.y = json_object_object_get_as_double(obj_json_obj, "height");
      name   = json_object_object_get_as_string(obj_json_obj, "name");

      props_json_obj = json_object_object_get(obj_json_obj, "properties");

      next_map  = json_object_object_get_as_string(props_json_obj, "next_map");
      link_with = json_object_object_get_as_string(props_json_obj, "link_with");

      make_ladder(ecs, pos, size, name, next_map, link_with);
    }
  }
  return 0;
}

static int parse_layer(const json_object* layer_json_obj, Ecs* ecs)
{
  const char* layer_type;
  INFO("layer name: %s\n", json_object_get_string(json_object_object_get(layer_json_obj, "name")));

  layer_type = json_object_get_string(json_object_object_get(layer_json_obj, "type"));
  if (strcmp(layer_type, "tilelayer") == 0)
  {
    parse_tilelayer(layer_json_obj);
  }
  else if (strcmp(layer_type, "objectgroup") == 0)
  {
    parse_objectgroup(layer_json_obj, ecs);
  }

  return 0;
}

static int parse(const json_object* map_json_obj, Ecs* ecs)
{
  const json_object* layers_json_obj;
  const json_object* layer_json_obj;
  int                cnt, w, h;
  layers_json_obj = json_object_object_get(map_json_obj, "layers");
  cnt             = json_object_array_length(layers_json_obj);
  w               = json_object_get_int(json_object_object_get(map_json_obj, "width"));
  h               = json_object_get_int(json_object_object_get(map_json_obj, "height"));
  map_set_size(w, h);
  for (int i = 0; i < cnt; ++i)
  {
    layer_json_obj = json_object_array_get_idx(layers_json_obj, i);
    parse_layer(layer_json_obj, ecs);
  }
  return 0;
}

int load_map_from_json_file(const char* filename, Ecs* ecs)
{
  char*        input;
  size_t       inplen;
  json_object* json_obj;
  FILE*        file;

  if ((file = fopen(filename, "r")) == NULL)
    return -1;

  if (readall(file, &input, &inplen) != 0)
  {
    fclose(file);
    file = NULL;
    return -1;
  }

  json_obj = json_tokener_parse(input);

  parse(json_obj, ecs);
  fclose(file);
  json_object_put(json_obj);
  free(input);
  return 0;
}
