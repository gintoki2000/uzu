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
#include "ui_dialogue.h"
#include "ui_list.h"
#include "ui_msgbox.h"
#include "ui_quality.h"
#include "ecs/ecs.h"

#include "engine/keyboard.h"

#include "system/ai_system.h"
#include "system/animator_ctl_sys.h"
#include "system/animator_sys.h"
#include "system/block_input_system.h"
#include "system/camera_sys.h"
#include "system/casting_sys.h"
#include "system/chest_system.h"
#include "system/collision_mgr_sys.h"
#include "system/collision_sys.h"
#include "system/dialogue_sys.h"
#include "system/dmg_sys.h"
#include "system/door_system.h"
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
#include "system/self_destruction.h"
#include "system/text_rendering_sys.h"
#include "system/tile_collision_sys.h"

#include "system/weapon_skill/charge.h"
#include "system/weapon_skill/swing.h"
#include "system/weapon_skill/thunder_storm.h"
#include "system/weapon_skill/thust.h"

#include "game_event/game_event.h"

#include "system/debug/draw_hitbox.h"
#include "system/debug/draw_path.h"
#include "system/debug/draw_position.h"
#include "system/debug/draw_target.h"

#include <json-c/json.h>

#include "../include/entity_utils.h"
#include "json_helper.h"
#include "system/event_messaging_sys.h"

static void on_load(void);
static void on_unload(void);
static void on_event(const SDL_Event* evt);
static void on_update();
static void on_player_hit_ladder(pointer_t arg, const MSG_HitLadder* event);
static void on_entity_died(pointer_t arg, const MSG_EntityDied* event);
static void unload_current_level(void);

static void __callback_clear_entities(pointer_t arg, Ecs* ecs, ecs_entity_t entity);

static void music_player_on_level_loaded(void* arg, const MSG_LevelLoaded* event);

extern EcsType g_comp_types[];

const Scene g_game_scene = {
  .on_load   = on_load,
  .on_unload = on_unload,
  .on_event  = on_event,
  .on_update = on_update,
};

Ecs* g_ecs;

static BOOL _has_next_level;
static char _next_level[LEVEL_SWITCHER_MAX_LEVEL_NAME_LEN + 1];
static char _spwan_location[LEVEL_SWITCHER_MAX_DEST_LEN + 1];
static BOOL _paused;
static char _current_level_name[LEVEL_SWITCHER_MAX_LEVEL_NAME_LEN + 1];

static void spawn_player(Vec2 pos)
{
  ecs_entity_t player;
  ecs_entity_t weapon;

  // TODO: di chuyển data của player + vũ khí từ ecs registry tạm về lại ecs registry chính

  player = g_char_create_fn_tbl[g_session.job](g_ecs, pos);
  weapon =
      g_weapon_create_fn_tbl[g_session.weapon](g_ecs,
                                               BIT(CATEGORY_ENEMY) | BIT(CATEGORY_INTERACABLE));

  make_player(g_ecs, player, weapon);

  set_entity_hit_points(g_ecs, player, g_session.hp);
  set_entity_mana_points(g_ecs, player, g_session.mp);
  set_spell(g_ecs, player, g_session.spell);
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
  door_system_init();
  chest_system_init();
  player_controller_system_init();

  // init ui
  ui_dialogue_init();

  ems_connect(MSG_HIT_LADDER, NULL, on_player_hit_ladder);
  ems_connect(MSG_ENTITY_DIED, NULL, on_entity_died);
  ems_connect(MSG_LEVEL_LOADED, NULL, music_player_on_level_loaded);

  keybroad_push_state(player_controller_system_update);

  if (g_session.new_game)
  {
    load_level("0");
    ems_broadcast(MSG_NEW_GAME, NULL);
    ems_broadcast(MSG_LEVEL_LOADED, &(MSG_LevelLoaded){ "0" });
    spawn_player(g_session.pos);
    add_to_inv(ITEM_TYPE_ANIME_SWORD, 1);
    add_to_inv(ITEM_TYPE_CLEAVER, 1);
    add_to_inv(ITEM_TYPE_SPEAR, 1);
    add_to_inv(ITEM_TYPE_STAFF, 1);
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

static void on_event(const SDL_UNUSED SDL_Event* evt)
{
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
    ems_broadcast(MSG_LEVEL_LOADED, &(MSG_LevelLoaded){ _next_level });

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
      input_blocking_system();
      self_destruction_system();

      //  skl
      swing_weapon_skl_system_update();
      casting_system_update();
      charge_weapon_skl_system();

      weapon_skill_thust_update();
      thunder_storm_weapon_skl_system_update();
    }
    ui_dialogue_update();

    // render
#if 1
    map_draw(MAP_LAYER_FLOOR);
    map_draw(MAP_LAYER_BACK_WALL);
    rendering_system_update();
    healthbar_rendering_system_update();
    map_draw(MAP_LAYER_FRONT_WALL);
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
#endif

#if 0
    // render debug
    // collision_system_render_debug();
    // path_rendering_system_update();
    // move_target_rendering_system_update();
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

static void on_entity_died(SDL_UNUSED pointer_t arg, SDL_UNUSED const MSG_EntityDied* event)
{
}

static void music_player_on_level_loaded(SDL_UNUSED void* arg, const MSG_LevelLoaded* event)
{
  u16 musid = BG_MUS_ID_NULL;
  if (strcmp(event->level_name, "0") == 0)
    musid = BG_MUS_LV1;
  else if (strcmp(event->level_name, "1") == 0)
    musid = BG_MUS_LV2;
  if (musid != BG_MUS_ID_NULL)
    Mix_PlayMusic(get_bg_mus(musid), -1);
}

static void __callback_clear_entities(pointer_t arg, Ecs* ecs, ecs_entity_t entity)
{
  (void)arg;
  ecs_destroy(ecs, entity);
}

static void unload_current_level()
{
  // TODO: copy dữ liệu của player sang ecs registry tạm
  ecs_entity_t player = get_player(g_ecs);
  g_session.hp        = get_entity_hit_points(g_ecs, player);
  g_session.mp        = get_entity_mana_points(g_ecs, player);
  g_session.spell     = get_spell(g_ecs, player);
  g_session.weapon    = get_equiped_weapon_type_id(g_ecs, player);
  ecs_each(g_ecs, NULL, __callback_clear_entities);
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
