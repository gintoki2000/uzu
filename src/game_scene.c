#include "game_scene.h"
#include "components.h"
#include "entity_factory.h"
#include "map.h"
#include "read_all.h"
#include "scene.h"

#include "ecs/ecs.h"

#include "system/ai_system.h"
#include "system/animator_ctl_sys.h"
#include "system/animator_sys.h"
#include "system/camera_sys.h"
#include "system/collision_mgr_sys.h"
#include "system/collision_sys.h"
#include "system/dmg_sys.h"
#include "system/drop_system.h"
#include "system/equipment_sys.h"
#include "system/health_sys.h"
#include "system/healthbar_rendering_sys.h"
#include "system/hud_system.h"
#include "system/late_destroying_sys.h"
#include "system/life_span_sys.h"
#include "system/mediator.h"
#include "system/motion_sys.h"
#include "system/pickup_sys.h"
#include "system/player_ctl_sys.h"
#include "system/rendering_sys.h"
#include "system/tile_collision_sys.h"

#include "system/weapon_skill/charge.h"
#include "system/weapon_skill/swing.h"
#include "system/weapon_skill/thunder_storm.h"

#include "system/debug/draw_hitbox.h"
#include "system/debug/draw_path.h"
#include "system/debug/draw_position.h"
#include "system/debug/draw_target.h"

#include <json-c/json.h>

#define json_object_object_get_as_string(__jobj, __key)                                            \
  json_object_get_string(json_object_object_get(__jobj, __key))
#define json_object_object_get_as_double(__jobj, __key)                                            \
  json_object_get_double(json_object_object_get(__jobj, __key))
#define json_object_object_get_as_int(__jobj, __key)                                               \
  json_object_get_int(json_object_object_get(__jobj, __key))
#define json_object_array_get_idx_as_int(__jarr, __idx)                                            \
  json_object_get_int(json_object_array_get_idx(__jarr, __idx))

static void         on_load();
static void         on_unload();
static void         on_event(const SDL_Event* evt);
static void         on_update();
static void         on_player_hit_ladder(void* arg, const SysEvt_HitLadder* event);
static void         emit_signal(int sig_id, const pointer_t event);
static void         load_map(const char* filename);
static json_object* load_json_from_file(const char* filename);

static int parse_tilelayer(const json_object* tilelayer_json_obj);
static int parse_objectgroup(const json_object* object_group_json_obj);
static int parse(const json_object* map_json_obj);

extern EcsType g_comp_types[];

const Scene g_game_scene = {
  .on_load   = on_load,
  .on_unload = on_unload,
  .on_event  = on_event,
  .on_update = on_update,
};

Ecs* g_ecs;

static Dispatcher* _dispatcher;
static BOOL        _load_next_map;

static void on_load()
{
  _dispatcher = dispatcher_new(GAME_SCENE_SIG_CNT);

  mediator_init();
  collision_system_init();
  health_system_init();
  drop_system_init();
  pickup_system_init();
  damage_system_init();
  collision_manager_system_init();

  mediator_connect(SYS_SIG_HIT_LADDER, NULL, SLOT(on_player_hit_ladder));

  load_map("asserts/level/0.json");
}

static void on_unload()
{
  ecs_del(g_ecs);
  dispatcher_destroy(_dispatcher);

  g_ecs       = NULL;
  _dispatcher = NULL;
}

static void on_event(const SDL_Event* evt)
{
  (void)evt;
}

static void on_update()
{
  if (_load_next_map)
  {

    _load_next_map = FALSE;
  }
  else
  {
    // update
    player_controller_system_update();
    motion_system_update();
    tile_collision_system_update();
    collision_system_update();
    equipment_system_update();
    animator_controller_system_update();
    animator_system_update();
    ai_system_update();
    health_system_update();
    camera_system_update();
    map_update_animated_cells();

    //  skl
    swing_weapon_skl_system_update();
    charge_weapon_skl_system();
    thunder_storm_weapon_skl_system_update();

    // render
    map_draw(MAP_LAYER_FLOOR);
    map_draw(MAP_LAYER_WALL);
    rendering_system_update();
    healthbar_rendering_system_update();
    map_draw(MAP_LAYER_FRONT);
    hub_system_update();

    // render debug
    collision_system_render_debug();
    path_rendering_system_update();
    hitbox_rendering_system_update();
    position_rendering_system_update();

    // late update
    late_destroying_system_update();
    life_span_system_update();
  }
}

static void on_player_hit_ladder(void* arg, const SysEvt_HitLadder* event)
{
  (void)arg;

  INFO("\n");
}

static void emit_signal(int sig_id, const pointer_t event)
{
  dispatcher_emit(_dispatcher, sig_id, event);
}

static void load_map(const char* file)
{
  json_object* json_map = NULL;

  if (g_ecs != NULL)
  {
    ecs_del(g_ecs);
  }

  g_ecs = ecs_new(g_comp_types, NUM_COMPONENTS);

  emit_signal(GAME_SCENE_SIG_BEGIN_LOAD_MAP, NULL);
  if ((json_map = load_json_from_file(file)) != NULL)
  {
    parse(json_map);
    free(json_map);
  }
}

#define FLOOR_LAYER_NAME "floor"
#define FRONT_WALL_LAYER_NAME "front-wall"
#define BACK_WALL_LAYER_NAME "back-wall"

static int layer_name_to_id(const char* name)
{
  if (strcmp(name, FLOOR_LAYER_NAME) == 0)
  {
    return MAP_LAYER_FLOOR;
  }
  else if (strcmp(name, FRONT_WALL_LAYER_NAME) == 0)
  {
    return MAP_LAYER_FRONT;
  }
  else if (strcmp(name, BACK_WALL_LAYER_NAME) == 0)
  {
    return MAP_LAYER_WALL;
  }
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
    else if (strcmp(objtype, "Player") == 0)
    {
      make_player(g_ecs, make_knight(g_ecs, pos), make_golden_sword(g_ecs, BIT(CATEGORY_ENEMY)));
    }
  }
  return 0;
}

static int parse_layer(const json_object* layer_json_obj)
{
  const char* layer_type;
  INFO("layer name: %s\n", json_object_object_get_as_string(layer_json_obj, "name"));

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

static json_object* load_json_from_file(const char* filename)
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

void game_scene_connect_sig(int sig_id, slot_t slot, pointer_t arg)
{
  dispatcher_connect(_dispatcher, sig_id, arg, slot);
}
