#include "level_loader.h"
#include "ecs/ecs.h"
#include "entity_factory.h"
#include "json-c/json.h"

#include "entity_utils.h"
#include "global.h"
#include "json_helper.h"
#include "map.h"

#define LEVEL_DATA_DIR "res/level/"

extern Ecs* g_ecs;

typedef struct EntityProperties
{
  Vec2         position;
  Vec2         size;
  const char*  name;
  json_object* properties;
  u16          id;
} EntityProperties;

//<Helper functions>//
static char* _layer_name_tbl[NUM_MAP_LAYERS] = {
  [MAP_LAYER_FLOOR]      = "floor",
  [MAP_LAYER_BACK_WALL]  = "back-wall",
  [MAP_LAYER_FRONT_WALL] = "front-wall",
};

static void level_name_to_file_name(const char* level_name, char* dest)
{
  strcpy(dest, LEVEL_DATA_DIR);
  strcat(dest, level_name);
  strcat(dest, ".json");
}

static int item_id_from_string(const char* name)
{
  static const char* lut[NUM_ITEM_TYPES] = {
    [ITEM_TYPE_RED_FLASK]        = "RedFlask",
    [ITEM_TYPE_BIG_RED_FLASK]    = "BigRedFlask",
    [ITEM_TYPE_BLUE_FLASK]       = "BlueFlask",
    [ITEM_TYPE_SCROLL_ICE_ARROW] = "IceArrowScroll",
    [ITEM_TYPE_SCROLL_FIRE_BALL] = "FireBallScroll",
    [ITEM_TYPE_KEY_1_1]          = "Key_1_1",
    [ITEM_TYPE_STAFF]            = "RedStaff",
  };
  for (int i = 0; i < NUM_ITEM_TYPES; ++i)
    if (lut[i] && SDL_strcmp(lut[i], name) == 0)
      return i;
  return -1;
}

static int layer_name_to_id(const char* name)
{
  for (int i = 0; i < NUM_MAP_LAYERS; ++i)
    if (SDL_strcmp(_layer_name_tbl[i], name) == 0)
      return i;
  return -1;
}
//**********************************************************************//

static int parse_tilelayer(const json_object* tilelayer_json_obj);
static int parse_objectgroup(const json_object* object_group_json_obj);

static void parse_imp(Ecs* registry, const EntityProperties* params);
static void parse_wogol(Ecs* registry, const EntityProperties* params);
static void parse_huge_demon(Ecs* registry, const EntityProperties* params);
static void parse_chest(Ecs* registry, const EntityProperties* params);
static void parse_ladder(Ecs* registry, const EntityProperties* params);
static void parse_chort(Ecs* registry, const EntityProperties* params);
static void parse_door(Ecs* registry, const EntityProperties* params);

static void parse_item(Item* item, json_object* json)
{
  ASSERT(json != NULL);
  const char* item_name;
  int         num_items;

  item_name       = json_object_object_get_as_string(json, "type");
  num_items       = json_object_object_get_as_int(json, "count");
  item->type_id   = item_id_from_string(item_name);
  item->num_items = num_items;
}

static void parse_item_list(Item items[5], u16* num_items, const char* input)
{
  json_object* json = NULL;

  int n;
  if (input == NULL)
    return;

  json = json_tokener_parse(input);

  n = json_object_array_length(json);

  n = min(CHEST_MAX_ITEMS, n);
  for (int i = 0; i < n; ++i)
  {
    parse_item(&items[i], json_object_array_get_idx(json, i));
  }

  *num_items = n;
  json_object_put(json);
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
    map_scan_animated_cells(layer);
  }

  free(data);

  return 0;
}

static void (*get_entity_create_fn(const char* entity_type_name))(Ecs*, const EntityProperties*)
{
  struct
  {
    const char* name;
    void (*const fn)(Ecs*, const EntityProperties*);
  } static lut[] = {
    { "Imp", parse_imp },     { "Wogol", parse_wogol },   { "BigDemon", parse_huge_demon },
    { "Chest", parse_chest }, { "Ladder", parse_ladder }, { "Chort", parse_chort },
    { "Door", parse_door }
  };

  for (int i = 0; lut[i].name != NULL; ++i)
    if (!SDL_strcmp(lut[i].name, entity_type_name))
      return lut[i].fn;
  return NULL;
}

static int parse_objectgroup(const json_object* object_group_json_obj)
{
  const json_object* objects_json_obj;
  int                objcnt;
  const char*        objtype;
  const json_object* obj_json_obj;

  EntityProperties params;
  void (*parse_fn)(Ecs*, const EntityProperties*);

  objects_json_obj = json_object_object_get(object_group_json_obj, "objects");
  objcnt           = json_object_array_length(objects_json_obj);

  for (int i = 0; i < objcnt; ++i)
  {
    obj_json_obj = json_object_array_get_idx(objects_json_obj, i);

    // common attributes
    objtype           = json_object_object_get_as_string(obj_json_obj, "type");
    params.position.x = json_object_object_get_as_double(obj_json_obj, "x");
    params.position.y = json_object_object_get_as_double(obj_json_obj, "y");
    params.size.x     = json_object_object_get_as_double(obj_json_obj, "width");
    params.size.y     = json_object_object_get_as_double(obj_json_obj, "height");
    params.name       = json_object_object_get_as_string(obj_json_obj, "name");
    params.properties = json_object_object_get(obj_json_obj, "properties");
    params.id         = json_object_object_get_as_int(obj_json_obj, "id");

    if ((parse_fn = get_entity_create_fn(objtype)) != NULL)
      parse_fn(g_ecs, &params);
  }

  return 0;
}

static int parse_layer(const json_object* layer_json_obj)
{
  const char* layer_type;

  layer_type = json_object_object_get_as_string(layer_json_obj, "type");
  if (SDL_strcmp(layer_type, "tilelayer") == 0)
  {
    parse_tilelayer(layer_json_obj);
  }
  else if (SDL_strcmp(layer_type, "objectgroup") == 0)
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
  json_object_put(json_map_data);
  return LOAD_LEVEL_OK;
}

static Vec2 real_position(Vec2 top_left, Vec2 size)
{
  Vec2 result;
  result.x = top_left.x + size.x / 2.f;
  result.y = top_left.y + size.y;
  return result;
}
//*****************************************************************************//
static void parse_imp(Ecs* registry, const EntityProperties* props)
{
  make_imp(registry, real_position(props->position, props->size));
}
static void parse_wogol(Ecs* registry, const EntityProperties* props)
{
  make_wogol(registry, real_position(props->position, props->size));
}
static void parse_huge_demon(Ecs* registry, const EntityProperties* props)
{
  make_huge_demon(registry, real_position(props->position, props->size));
}
static void parse_chest(Ecs* registry, const EntityProperties* props)
{
  NewChestParams chest_params;
  chest_params.position = real_position(props->position, props->size);
  chest_params.id       = props->id;
  chest_params.state    = CHEST_STATE_CLOSE;

  parse_item_list(chest_params.items,
                  &chest_params.num_slots,
                  json_object_object_get_as_string(props->properties, "items"));
  make_chest(registry, &chest_params);
}

static int direction_from_string(const char* value)
{

  static struct
  {
    const char* string_value;
    u16         int_value;
  } lut[4] = { { "up", UP }, { "down", DOWN }, { "left", LEFT }, { "right", RIGHT } };

  for (int i = 0; i < 4; ++i)
  {
    if (SDL_strcmp(value, lut[i].string_value) == 0)
      return lut[i].int_value;
  }

  ASSERT(FALSE && "invalid value");
  return -1;
}

static void parse_ladder(Ecs* registry, const EntityProperties* props)
{
  const char* direction;

  direction = json_object_object_get_as_string(props->properties, "direction");
  NewLadderParams params;
  params.level     = json_object_object_get_as_string(props->properties, "level");
  params.dest      = json_object_object_get_as_string(props->properties, "dest");
  params.name      = props->name;
  params.position  = props->position;
  params.size      = props->size;
  params.direction = direction_from_string(direction);

  make_ladder(registry, &params);
}

static void parse_chort(Ecs* registry, const EntityProperties* params)
{
  make_chort(registry, real_position(params->position, params->size));
}

static void parse_door(Ecs* registry, const EntityProperties* params)
{
  make_door(registry, real_position(params->position, params->size));
}
