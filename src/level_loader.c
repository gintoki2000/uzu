#include "level_loader.h"
#include "ecs/ecs.h"
#include "entity_factory.h"
#include "json-c/json.h"
#include "json_helper.h"
#include "map.h"
#include "types.h"
#include "utils.h"

extern Ecs* g_ecs;

static void level_name_to_file_name(const char* level_name, char* dest)
{
  strcpy(dest, "asserts/level/");
  strcat(dest, level_name);
  strcat(dest, ".json");
}

static char* _layer_name_tbl[NUM_MAP_LAYERS] = {
  [MAP_LAYER_FLOOR] = "floor",
  [MAP_LAYER_WALL]  = "back-wall",
  [MAP_LAYER_FRONT] = "front-wall",
};

static int layer_name_to_id(const char* name)
{
  for (int i = 0; i < NUM_MAP_LAYERS; ++i)
    if (strcmp(_layer_name_tbl[i], name) == 0)
      return i;
  return -1;
}

static int parse_tilelayer(const json_object* tilelayer_json_obj)
{
  int*               data;
  int                datalen;
  const json_object* data_json_obj;
  const char*        name;
  int                layer;

  data_json_obj = json_object_object_get(tilelayer_json_obj, "data");
  name          = json_object_object_get_as_string(tilelayer_json_obj, "name");
  datalen       = json_object_array_length(data_json_obj);

  data = malloc(datalen * sizeof(int));

  for (int i = 0; i < datalen; ++i)
  {
    data[i] = json_object_array_get_idx_as_int(data_json_obj, i);
  }

  if ((layer = layer_name_to_id(name)) != -1)
  {
    map_set_data(layer, data, datalen);
  }

  free(data);

  return 0;
}

static int parse_objectgroup(const json_object* object_group_json_obj)
{
  const json_object* objects_json_obj;
  const json_object* props_json_obj;
  int                objcnt;
  const char*        objtype;
  const json_object* obj_json_obj;
  Vec2               pos;
  Vec2               size;
  const char*        name;

  objects_json_obj = json_object_object_get(object_group_json_obj, "objects");
  objcnt           = json_object_array_length(objects_json_obj);

  for (int i = 0; i < objcnt; ++i)
  {
    obj_json_obj = json_object_array_get_idx(objects_json_obj, i);

    // common attributes
    objtype = json_object_object_get_as_string(obj_json_obj, "type");
    pos.x   = json_object_object_get_as_double(obj_json_obj, "x");
    pos.y   = json_object_object_get_as_double(obj_json_obj, "y");
    size.x  = json_object_object_get_as_double(obj_json_obj, "width");
    size.y  = json_object_object_get_as_double(obj_json_obj, "height");
    name    = json_object_object_get_as_string(obj_json_obj, "name");

    if (strcmp(objtype, "Chort") == 0)
    {
      make_chort(g_ecs, pos);
    }
    else if (strcmp(objtype, "Ladder") == 0)
    {
      const char* level;
      const char* dest;

      props_json_obj = json_object_object_get(obj_json_obj, "properties");

      level = json_object_object_get_as_string(props_json_obj, "level");
      dest  = json_object_object_get_as_string(props_json_obj, "dest");

      make_ladder(g_ecs, name, pos, size, level, dest);
    }
    else if (strcmp(objtype, "BigDemon") == 0)
    {
      make_huge_demon(g_ecs, pos);
    }
    else if (strcmp(objtype, "NPC") == 0)
    {
      make_wizzard_npc(g_ecs, VEC2(pos.x + size.x / 2.f, pos.y + size.y));
    }
    else if (strcmp(objtype, "Wogol") == 0)
    {
      make_wogol(g_ecs, pos);
    }
    else if (strcmp(objtype, "Imp") == 0)
    {
      make_imp(g_ecs, pos); 
    }
    else if (strcmp(objtype, "Door") == 0) 
    {
      make_door(g_ecs, VEC2(pos.x + size.x / 2, pos.y + size.y));
    }
  }
  return 0;
}

static int parse_layer(const json_object* layer_json_obj)
{
  const char* layer_type;

  layer_type = json_object_object_get_as_string(layer_json_obj, "type");
  if (strcmp(layer_type, "tilelayer") == 0)
  {
    parse_tilelayer(layer_json_obj);
  }
  else if (strcmp(layer_type, "objectgroup") == 0)
  {
    parse_objectgroup(layer_json_obj);
  }

  return 0;
}

static int parse(const json_object* map_json_obj)
{
  const json_object* layers_json_obj;
  const json_object* layer_json_obj;
  int                cnt, w, h;
  layers_json_obj = json_object_object_get(map_json_obj, "layers");
  cnt             = json_object_array_length(layers_json_obj);
  w               = json_object_object_get_as_int(map_json_obj, "width");
  h               = json_object_object_get_as_int(map_json_obj, "height");
  map_set_size(w, h);
  for (int i = 0; i < cnt; ++i)
  {
    layer_json_obj = json_object_array_get_idx(layers_json_obj, i);
    parse_layer(layer_json_obj);
  }
  return 0;
}

int load_level(const char* level_name)
{
  char                filename[255];
  struct json_object* json_map_data;
  level_name_to_file_name(level_name, filename);

  if ((json_map_data = load_json_from_file(filename)) == NULL)
    return LOAD_LEVEL_FAIL_TO_LOAD_FILE;
  parse(json_map_data);
  return LOAD_LEVEL_OK;
}
