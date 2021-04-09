#include "level_loader.h"
#include "ecs/ecs.h"
#include "entity_factory.h"
#include "json-c/json.h"

#include "entity_utils.h"
#include "global.h"
#include "json_helper.h"
#include "map.h"

#define LEVEL_DATA_DIR "res/level/"
#define JV(json_object, type, name) json_object_object_get_as_##type(json_object, name)

extern Ecs* g_ecs;

typedef struct EntityProperties
{
  Vec2         position;
  Vec2         size;
  const char*  name;
  json_object* properties;
  u16          id;
} EntityProperties;

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

//**********************************************************************//

static int parse_tilelayer(const json_object* tilelayer_json_obj, tile_t* data);
static int parse_objectgroup(const json_object* jobjectgroup);

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

  item_name       = JV(json, string, "type"   );
  num_items       = JV(json, int   , "quality");
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

static json_object* find_layer(json_object* layers, const char* layer_name)
{
  int          nlayer = json_object_array_length(layers);
  json_object* jlayer;
  for (int i = 0; i < nlayer; ++i)
  {
    jlayer = json_object_array_get_idx(layers, i);
    if (!SDL_strcmp(layer_name, JV(jlayer, string, "name")))
      return jlayer;
  }
  return NULL;
}

static int parse_tilelayer(const json_object* tilelayer_json_obj, tile_t* data)
{
  int                datalen;
  const json_object* data_json_obj;

  data_json_obj = json_object_object_get(tilelayer_json_obj, "data");
  datalen       = json_object_array_length(data_json_obj);

  for (int i = 0; i < datalen; ++i)
    data[i] = json_object_array_get_idx_as_int(data_json_obj, i);

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

static int parse_objectgroup(const json_object* jobjectgroup)
{
  const json_object* jobjects;
  int                objcnt;
  const char*        objtype;
  const json_object* jobj;

  EntityProperties params;
  void (*parse_fn)(Ecs*, const EntityProperties*);

  jobjects = json_object_object_get(jobjectgroup, "objects");
  objcnt   = json_object_array_length(jobjects);

  for (int i = 0; i < objcnt; ++i)
  {
    jobj = json_object_array_get_idx(jobjects, i);

    // common attributes
    objtype           = json_object_object_get_as_string(jobj, "type");
    params.position.x = json_object_object_get_as_double(jobj, "x");
    params.position.y = json_object_object_get_as_double(jobj, "y");
    params.size.x     = json_object_object_get_as_double(jobj, "width");
    params.size.y     = json_object_object_get_as_double(jobj, "height");
    params.name       = json_object_object_get_as_string(jobj, "name");
    params.properties = json_object_object_get(jobj, "properties");
    params.id         = json_object_object_get_as_int(jobj, "id");

    if ((parse_fn = get_entity_create_fn(objtype)) != NULL)
      parse_fn(g_ecs, &params);
  }

  return 0;
}

static int parse(const json_object* jmap)
{
  json_object *jlayers, *jentities_layer, *jwall_layer, *jfloor_layer;
  int          w, h;
  tile_t*      floor;
  tile_t*      wall;

  jlayers = json_object_object_get(jmap, "layers");
  w       = JV(jmap, int, "width");
  h       = JV(jmap, int, "height");

  floor = SDL_malloc(w * h * sizeof(tile_t));
  wall  = SDL_malloc(w * h * sizeof(tile_t));

  jentities_layer = find_layer(jlayers, "entities");
  jwall_layer     = find_layer(jlayers, "wall");
  jfloor_layer    = find_layer(jlayers, "floor");

  ASSERT(jentities_layer != NULL && STREQ(JV(jentities_layer, string, "type"), "objectgroup"));
  ASSERT(jfloor_layer != NULL && STREQ(JV(jfloor_layer, string, "type"), "tilelayer"));
  ASSERT(jwall_layer != NULL && STREQ(JV(jwall_layer, string, "type"), "tilelayer"));
  parse_objectgroup(jentities_layer);
  parse_tilelayer(jfloor_layer, floor);
  parse_tilelayer(jwall_layer, wall);

  map_load(floor, wall, w, h);

  SDL_free(floor);
  SDL_free(wall);

  return 0;
}

#undef JV

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
