#include "game_scene.h"
#include "SDL_mixer.h"
#include "camera_shaker.h"
#include "components.h"
#include "config.h"
#include "constances.h"
#include "ecs/ecs.h"
#include "engine/engine.h"
#include "entity_factory.h"
#include "inventory.h"
#include "level_loader.h"
#include "map.h"
#include "resources.h"
#include "scene.h"
#include "session.h"
#include "sprite_renderer.h"
#include "ui_dialogue.h"
#include "ui_list.h"
#include "ui_msgbox.h"
#include "ui_msgbox_w_icon.h"
#include "ui_quality.h"
#include "ui_subtile.h"

#include "engine/keyboard.h"

#include "system/collision_sys.h"
#include "system/game_logic.h"
#include "system/rendering.h"
#include "system/weapon_skills.h"

#include "game_event.h"

#include "dungeon.h"
#include "entity_utils.h"
#include "system/debug/draw_hitbox.h"
#include "system/debug/draw_path.h"
#include "system/debug/draw_position.h"
#include "system/debug/draw_target.h"
#include "system/event_messaging_sys.h"

static void on_player_hit_ladder(void* arg, const MSG_HitLadder* event);
static void on_entity_died(void* arg, const MSG_EntityDied* event);
static void unload_current_level(void);
static void update_ui(void);
static void render_ui(void);
static void update_game_logic(void);
static void render_game_world(void);
static void next_level(void);

static void music_player_on_level_loaded(void* arg, const MSG_LevelLoaded* event);

extern EcsType g_comp_types[];

DEFINE_SCENE(game);

Ecs* g_ecs;

static BOOL _has_next_level;
static char _next_level[LADDER_ATTRS_MAX_LEVEL_NAME_LEN + 1];
static char _spwan_location[LADDER_ATTRS_MAX_DEST_LEN + 1];
static BOOL _paused;
static BOOL _player_died;

#if DEBUG
static BOOL _draw_tile_colliders;
static BOOL _draw_hitbox;
static BOOL _draw_position;
static BOOL _draw_rtree;
#endif

static void spawn_player(Vec2 position)
{
  ecs_entity_t player;
  ecs_entity_t (*make_character_fn)(Ecs*, Vec2);

  make_character_fn = g_make_character_fn_tbl[g_session.job];

  player = make_character_fn(g_ecs, position);

  make_player(g_ecs, player, make_weapon(g_ecs, g_session.weapon));

  ett_attune_spell(g_ecs, player, g_session.spell);
}
static void on_load()
{
  extern Cursor g_cursor_cross;
  push_cursor_state(g_cursor_cross);
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
  effect_system_init();
  update_attack_target_system_init();
  action_system_init();
  stats_system_init();
  stagger_system_init();
  status_effect_system_init();

  // init ui
  ui_dialogue_init();
  ui_subtile_init();
  ui_msgbox_w_icon_init();

  inventory_init();

  ems_connect(MSG_HIT_LADDER, CALLBACK_2(on_player_hit_ladder));
  ems_connect(MSG_ENTITY_DIED, CALLBACK_2(on_entity_died));
  ems_connect(MSG_LEVEL_LOADED, CALLBACK_2(music_player_on_level_loaded));

  input_push_state(INPUT_STATE_INST_1(player_process_input));

  if (g_session.new_game)
  {
    if (load_level(g_session.level) != LOAD_LEVEL_OK)
    {
      engine_stop();
      INFO("Fail to load file\n");
      return;
    }
    ems_broadcast(MSG_NEW_GAME, NULL);
    ems_broadcast(MSG_LEVEL_LOADED, &(MSG_LevelLoaded){ g_session.level });
    spawn_player(g_session.pos);
    g_session.new_game = FALSE;
  }
  else
  {
    inventory_load();
    load_level(g_session.level);
    ems_broadcast(MSG_LEVEL_LOADED, &(MSG_LevelLoaded){ g_session.level });
    spawn_player(g_session.pos);
  }
}

static void on_unload()
{
  inventory_save();
  ems_broadcast(MSG_GAME_SCENE_UNLOAD, NULL);
  dialogue_system_fini();
  game_event_fini();
  ecs_del(g_ecs);
  ems_fini();

  g_ecs = NULL;
}

static void on_event(const SDL_UNUSED SDL_Event* evt)
{
}

static void on_update()
{
#if DEBUG
  if (key_just_pressed(SDL_SCANCODE_H))
    _draw_hitbox = !_draw_hitbox;
  if (key_just_pressed(SDL_SCANCODE_J))
    _draw_position = !_draw_position;
  if (key_just_pressed(SDL_SCANCODE_K))
    _draw_tile_colliders = !_draw_tile_colliders;
  if (key_just_pressed(SDL_SCANCODE_L))
    _draw_rtree = !_draw_rtree;
#endif

  if (_has_next_level)
  {
    unload_current_level();
    next_level();
  }
  else
  {
    // update
    if (!_paused)
    {
      update_game_logic();
    }

    update_ui();
    render_game_world();
    render_ui();

#if DEBUG
    if (_draw_hitbox)
      hitbox_rendering_system_update();
    if (_draw_position)
      position_rendering_system_update();
    if (_draw_tile_colliders)
      draw_map_colliders();
    if (_draw_rtree)
      collision_system_render_debug();
#endif

    // late update
    late_destroying_system();
    life_span_system();
  }
}

static void on_player_hit_ladder(SDL_UNUSED void* arg, const MSG_HitLadder* event)
{
  LadderAttributes* attrs = ecs_get(g_ecs, event->ladder, LADDER_ATTRIBUTES);
  SDL_strlcpy(_next_level, attrs->level, sizeof(_next_level) - 1);
  SDL_strlcpy(_spwan_location, attrs->dest, sizeof(_spwan_location) - 1);
  _has_next_level = TRUE;
}

static void on_entity_died(SDL_UNUSED void* arg, const MSG_EntityDied* event)
{
  if (ecs_has(g_ecs, event->entity, PLAYER_TAG))
    _player_died = TRUE;
}

static Mix_Music* music_from_level_name(const char* level_name)
{
  static struct
  {
    const char* level_name;
    u16         mus_id;
  } lut[] = {
    { "central", BG_MUS_BOSS },
    { "corridor", BG_MUS_LV2 },
    { "demon_ruin", BG_MUS_LV1 },
    { 0 },
  };
  for (int i = 0; lut[i].level_name != NULL; ++i)
    if (STREQ(level_name, lut[i].level_name))
      return get_bg_mus(lut[i].mus_id);
  return NULL;
}

static void music_player_on_level_loaded(SDL_UNUSED void* arg, const MSG_LevelLoaded* event)
{
  Mix_Music* mus = music_from_level_name(event->level_name);
  if (mus != NULL)
    Mix_PlayMusic(mus, -1);
}

static void unload_current_level()
{
  ems_broadcast(MSG_LEVEL_UNLOADED, &(MSG_LevelUnloaded){ g_session.level });
  ecs_entity_t player = scn_get_player(g_ecs);
  g_session.spell     = ett_get_attuned_spell_type(g_ecs, player);
  g_session.weapon    = ett_get_equiped_weapon_type(g_ecs, player);
  ecs_clear(g_ecs);
  map_clear();
}

void game_scene_pause()
{
  _paused = TRUE;
  ems_broadcast(MSG_GAME_PAUSED, NULL);
}

void game_scene_resume()
{
  _paused = FALSE;
  ems_broadcast(MSG_GAME_RESUMED, NULL);
}
static void update_ui()
{
  ui_dialogue_update();
  ui_subtile_update();
  ui_msgbox_w_icon_update();
}

static void render_ui()
{
  inventory_draw();
  ui_list_draw();
  ui_msgbox_draw();
  ui_msgbox_w_icon_draw();
  ui_dialogue_draw();
  ui_quality_draw();
  ui_subtile_draw();
}
static void update_game_logic(void)
{
  RUN_SYSTEM(stats_system);
  RUN_SYSTEM(motion_system);
  RUN_SYSTEM(tile_collision_system);
  RUN_SYSTEM(collision_system);
  RUN_SYSTEM(equipment_system);
  RUN_SYSTEM(character_controller_system);
  RUN_SYSTEM(animator_system);
  RUN_SYSTEM(ai_system);
  RUN_SYSTEM(health_system);
  RUN_SYSTEM(camera_system);
  RUN_SYSTEM(following_system);
  RUN_SYSTEM(stagger_system);
  RUN_SYSTEM(update_attack_target_system);
  RUN_SYSTEM(update_facing_direction_system);
  RUN_SYSTEM(action_system);
  RUN_SYSTEM(status_effect_system);

  (map_update_animated_cells());

  RUN_SYSTEM(weapon_swing_attack_system);
  RUN_SYSTEM(weapon_casting_system);
  RUN_SYSTEM(weapon_thust_attack_system);
  RUN_SYSTEM(weapon_thunder_storm_release_system);
  RUN_SYSTEM(weapon_shoot_system);
}

static void render_game_world(void)
{
  camera_shaker_update();
  sprite_renderer_begin();
  map_draw();
  RUN_SYSTEM(rendering_system);
  RUN_SYSTEM(emoji_system);
  sprite_renderer_end();
  RUN_SYSTEM(healthbar_rendering_system);
  RUN_SYSTEM(text_rendering_system);
  RUN_SYSTEM(interactable_rendering_system);
  RUN_SYSTEM(hub_rendering_system);
  RUN_SYSTEM(dialogue_system);
  RUN_SYSTEM(merchant_system);
  RUN_SYSTEM(status_effect_rendering_system);
  camera_shaker_postupdate();
}

static Vec2 get_spawn_localtion(ecs_entity_t ladder)
{
  LadderAttributes* attrs    = ecs_get(g_ecs, ladder, LADDER_ATTRIBUTES);
  HitBox*           hixbox   = ecs_get(g_ecs, ladder, HITBOX);
  Vec2              position = ett_get_position(g_ecs, ladder);
  switch (attrs->exit_direction)
  {
  case UP:
    position.x += hixbox->size.x / 2.f;
    position.y -= 5;
    break;
  case DOWN:
    position.x += hixbox->size.x / 2.f;
    position.y += 30;
    break;
  case LEFT:
    position.x -= hixbox->size.x / 2.f + 5.f;
    position.y += hixbox->size.y / 2.f;
    break;
  case RIGHT:
    position.x += hixbox->size.x / 2.f + 5.f;
    position.y += hixbox->size.y / 2.f;
  }
  return position;
}

static void next_level(void)
{
  ecs_entity_t ladder;
  Vec2         spawn_localtion;
  load_level(_next_level);

  if ((ladder = scn_find_portal(g_ecs, _spwan_location)) != ECS_NULL_ENT)
  {
    spawn_localtion = get_spawn_localtion(ladder);
    spawn_player(spawn_localtion);
  }
  SDL_strlcpy(g_session.level, _next_level, 255);
  ems_broadcast(MSG_LEVEL_LOADED, &(MSG_LevelLoaded){ _next_level });

  _has_next_level = FALSE;
}
