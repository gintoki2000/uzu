#include "game_scene.h"
#include "SDL_mixer.h"
#include "components.h"
#include "constances.h"
#include "entity_factory.h"
#include "inventory.h"
#include "map.h"
#include "resources.h"
#include "scene.h"
#include "ui_list.h"
#include "ui_msgbox.h"
#include "ui_quality.h"
#include "utils.h"

#include "ecs/ecs.h"

#include "engine/keyboard.h"

#include "system/ai_system.h"
#include "system/animator_ctl_sys.h"
#include "system/animator_sys.h"
#include "system/camera_sys.h"
#include "system/collision_mgr_sys.h"
#include "system/collision_sys.h"
#include "system/dialogue_sys.h"
#include "system/dmg_sys.h"
#include "system/drop_system.h"
#include "system/equipment_sys.h"
#include "system/following_system.h"
#include "system/health_sys.h"
#include "system/healthbar_rendering_sys.h"
#include "system/hud_system.h"
#include "system/interaction_system.h"
#include "system/late_destroying_sys.h"
#include "system/life_span_sys.h"
#include "system/merchant_sys.h"
#include "system/motion_sys.h"
#include "system/pickup_sys.h"
#include "system/player_ctl_sys.h"
#include "system/rendering_sys.h"
#include "system/text_rendering_sys.h"
#include "system/tile_collision_sys.h"

#include "system/weapon_skill/charge.h"
#include "system/weapon_skill/swing.h"
#include "system/weapon_skill/thunder_storm.h"
#include "system/weapon_skill/thust.h"

#include "system/game_event/game_event.h"

#include "system/debug/draw_hitbox.h"
#include "system/debug/draw_target.h"
#include "system/debug/draw_path.h"
#include "system/debug/draw_position.h"
#include "system/debug/draw_target.h"

#include <json-c/json.h>

#include "json_helper.h"
#include "system/event_messaging_sys.h"

static void on_load(void);
static void on_unload(void);
static void on_event(const SDL_Event* evt);
static void on_update();
static void on_player_hit_ladder(pointer_t arg, const MSG_HitLadder* event);
static void on_entity_died(pointer_t arg, const MSG_EntityDied* event);
static void load_level(const char* filename, BOOL spawn_player);
static void unload_current_level(void);

static int parse_tilelayer(const json_object* tilelayer_json_obj);
static int parse_objectgroup(const json_object* object_group_json_obj, BOOL spawn_player);
static int parse(const json_object* map_json_obj, BOOL spawn_player);

static void cb_clear_world(pointer_t arg, Ecs* ecs, ecs_entity_t entity);

extern EcsType g_comp_types[];

const Scene g_game_scene = {
  .on_load   = on_load,
  .on_unload = on_unload,
  .on_event  = on_event,
  .on_update = on_update,
};

Ecs* g_ecs;

static Dispatcher* _dispatcher;
static BOOL        _switch_level;
static char        _level_to_switch[100];
static char        _spwan_location[100];
static BOOL        _paused;

static void on_load()
{
  g_ecs = ecs_new(g_comp_types, NUM_COMPONENTS);

  ems_init();
  collision_system_init();
  health_system_init();
  drop_system_init();
  pickup_system_init();
  damage_system_init();
  collision_manager_system_init();
  dialogue_system_init();
  game_event_init();
  merchant_system_init();

  ems_connect(MSG_HIT_LADDER, NULL, on_player_hit_ladder);
  ems_connect(MSG_ENTITY_DIED, NULL, on_entity_died);

  keybroad_push_state(player_controller_system_update);

  load_level("asserts/level/0.json", TRUE);

  // Mix_PlayMusic(get_bg_mus(BG_MUS_THE_ESSENSE_OF_GOOD_THINGS), -1);
}

static void on_unload()
{
  dialogue_system_fini();
  ecs_del(g_ecs);
  dispatcher_destroy(_dispatcher);
  ems_fini();

  g_ecs       = NULL;
  _dispatcher = NULL;
}

static void on_event(const SDL_Event* evt)
{
  (void)evt;
}

static void on_update()
{
  if (_switch_level)
  {

    unload_current_level();
    load_level(_level_to_switch, TRUE);

    ecs_entity_t ladder;
    Transform*   transform;

    if ((ladder = find_ladder(g_ecs, _spwan_location)) != ECS_NULL_ENT)
    {
      transform = ecs_get(g_ecs, ladder, TRANSFORM);
      move_player_to(g_ecs, VEC2(transform->pos.x + 8, transform->pos.y + 30));
    }

    _switch_level = FALSE;
  }
  else
  {
    // update
    if (!_paused)
    {
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
      following_system_update();

      //  skl
      swing_weapon_skl_system_update();
      charge_weapon_skl_system();

      weapon_skill_thust_update();
      thunder_storm_weapon_skl_system_update();
    }

    // render
    map_draw(MAP_LAYER_FLOOR);
    map_draw(MAP_LAYER_WALL);
    rendering_system_update();
    healthbar_rendering_system_update();
    map_draw(MAP_LAYER_FRONT);
    text_rendering_system_update();
    interactable_pointer_rendering_system_update();
    hub_system_update();
    ui_list_draw();
    dialogue_system_update();
    inventory_draw();
    merchant_system_update();
    ui_msgbox_draw();
    ui_quality_draw();

#if 1
    // render debug
    //collision_system_render_debug();
    //path_rendering_system_update();
    move_target_rendering_system_update();
    hitbox_rendering_system_update();
    position_rendering_system_update();
#endif

    // late update
    late_destroying_system_update();
    life_span_system_update();
  }
}

static void on_player_hit_ladder(pointer_t arg, const MSG_HitLadder* event)
{
  (void)arg;

  LevelSwitcher* lsw;

  lsw = ecs_get(g_ecs, event->ladder, LEVEL_SWITCHER);

  _switch_level = TRUE;

  strcpy(_level_to_switch, "asserts/level/");

  strcat(_level_to_switch, lsw->level);
  strcat(_level_to_switch, ".json");
  strcpy(_spwan_location, lsw->dest);
}

static void on_entity_died(pointer_t arg, const MSG_EntityDied* event)
{
  (void)arg;
  if (ecs_get(g_ecs, event->entity, PLAYER_TAG))
  {
    //
  }
}

static void cb_clear_world(pointer_t arg, Ecs* ecs, ecs_entity_t entity)
{
  (void)arg;
  ecs_destroy(ecs, entity);
}

static void load_level(const char* file, BOOL spawn_player)
{
  json_object* json_map = NULL;
  if ((json_map = load_json_from_file(file)) != NULL)
  {
    parse(json_map, spawn_player);
    ems_broadcast(MSG_GAME_SCENE_LOADED, NULL);
    json_object_put(json_map);
  }
}

static void unload_current_level()
{
  ecs_each(g_ecs, NULL, cb_clear_world);
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

static int parse_objectgroup(const json_object* object_group_json_obj, BOOL spawn_player)
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
    else if (spawn_player && strcmp(objtype, "Player") == 0)
    {
      make_player(g_ecs, make_knight(g_ecs, pos), make_spear(g_ecs, BIT(CATEGORY_ENEMY)));
    }
    else if (strcmp(objtype, "NPC") == 0)
    {
      make_wizzard_npc(g_ecs, VEC2(pos.x + size.x / 2.f, pos.y + size.y));
    }
  }
  return 0;
}

static int parse_layer(const json_object* layer_json_obj, BOOL spawn_player)
{
  const char* layer_type;

  layer_type = json_object_object_get_as_string(layer_json_obj, "type");
  if (strcmp(layer_type, "tilelayer") == 0)
  {
    parse_tilelayer(layer_json_obj);
  }
  else if (strcmp(layer_type, "objectgroup") == 0)
  {
    parse_objectgroup(layer_json_obj, spawn_player);
  }

  return 0;
}

static int parse(const json_object* map_json_obj, BOOL spawn_player)
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
    parse_layer(layer_json_obj, spawn_player);
  }
  return 0;
}

void game_scene_pause()
{
  _paused = TRUE;
}

void game_scene_resume()
{
  _paused = FALSE;
}
