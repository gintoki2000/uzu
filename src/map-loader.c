#include "map-loader.h"
#include "map.h"
#include "read_all.h"
#include <json-c/json.h>
#include <json-c/json_visit.h>

static int parse_tilelayer(const json_object* tilelayer_json_obj)
{
  int*               data;
  int                datalen;
  const json_object* data_json_obj;
  const char*        name;
  int                to_layer;

  data_json_obj = json_object_object_get(tilelayer_json_obj, "data");
  name          = json_object_get_string(json_object_object_get(tilelayer_json_obj, "name"));
  datalen       = json_object_array_length(data_json_obj);

  data = malloc(datalen * sizeof(int));
  for (int i = 0; i < datalen; ++i)
  {
    data[i] = json_object_get_int(json_object_array_get_idx(data_json_obj, i));
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
  w = json_object_get_int(json_object_object_get(map_json_obj, "width"));
  h = json_object_get_int(json_object_object_get(map_json_obj, "height"));
  map_set_size(w, h);
  for (int i = 0; i < cnt; ++i)
  {
    layer_json_obj = json_object_array_get_idx(layers_json_obj, i);
    parse_layer(layer_json_obj, ecs);
  }
  return 0;
}

int load_map(const char* filename, Ecs* ecs)
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
  return 0;
}
