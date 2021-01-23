#include "game_scene.h"
#include "SDL_mixer.h"
#include "components.h"
#include "constances.h"
#include "entity_factory.h"
#include "inventory.h"
#include "level_loader.h"
#include "map.h"
#include "resources.h"
#include "scene.h"
#include "session.h"
#include "ui_list.h"
#include "ui_msgbox.h"
#include "ui_quality.h"
#include "ui_dialogue.h"
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
static void unload_current_level(void);

static void cb_clear_world(pointer_t arg, Ecs* ecs, ecs_entity_t entity);

extern EcsType g_comp_types[];

const Scene g_game_scene = {
  .on_load   = on_load,
  .on_unload = on_unload,
  .on_event  = on_event,
  .on_update = on_update,
};

Ecs* g_ecs;

static BOOL _has_next_level;
static char _next_level[100];
static char _spwan_location[100];
static BOOL _paused;

static void spawn_player(Vec2 pos)
{
  ecs_entity_t player;
  ecs_entity_t weapon;

  player = g_char_create_fn_tbl[g_session.job](g_ecs, pos);
  weapon = g_weapon_create_fn_tbl[g_session.weapon](g_ecs, BIT(CATEGORY_ENEMY));

  make_player(g_ecs, player, weapon);

  set_entity_hit_points(g_ecs, player, g_session.hp);
}
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

  //init ui
  ui_dialogue_init();

  ems_connect(MSG_HIT_LADDER, NULL, on_player_hit_ladder);
  ems_connect(MSG_ENTITY_DIED, NULL, on_entity_died);

  keybroad_push_state(player_controller_system_update);

  if (g_session.new_game)
  {
    load_level("0");
    ems_broadcast(MSG_NEW_GAME, NULL);
    ems_broadcast(MSG_LEVEL_LOADED, &(MSG_LevelLoaded){ "0" });
    spawn_player(g_session.pos);
  }
  else
  {
    load_level(g_session.level);
    ems_broadcast(MSG_LEVEL_LOADED, &(MSG_LevelLoaded){ g_session.level });
    spawn_player(g_session.pos);
  }
}

static void on_unload()
{
  ui_dialogue_fini();
  ems_broadcast(MSG_GAME_SCENE_UNLOAD, NULL);
  dialogue_system_fini();
  ecs_del(g_ecs);
  ems_fini();

  g_ecs = NULL;
}

static void on_event(const SDL_Event* evt)
{
  (void)evt;
}

static void on_update()
{
  if (_has_next_level)
  {

    unload_current_level();
    load_level(_next_level);

    ecs_entity_t ladder;

    if ((ladder = find_ladder(g_ecs, _spwan_location)) != ECS_NULL_ENT)
    {
      Vec2 pos = get_entity_position(g_ecs, ladder);
      pos.x += 8;
      pos.y += 30;
      spawn_player(pos);
    }
    ems_broadcast(MSG_LEVEL_LOADED, _next_level);

    _has_next_level = FALSE;
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
    ui_dialogue_update();

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
    ui_dialogue_draw();
    ui_quality_draw();
    

#if 1
    // render debug
    // collision_system_render_debug();
    // path_rendering_system_update();
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

  _has_next_level = TRUE;

  strcpy(_next_level, lsw->level);
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

static void unload_current_level()
{
  g_session.hp = get_entity_hit_points(g_ecs, get_player(g_ecs));
  ecs_each(g_ecs, NULL, cb_clear_world);
  map_clear();
}

void game_scene_pause()
{
  _paused = TRUE;
}

void game_scene_resume()
{
  _paused = FALSE;
}
