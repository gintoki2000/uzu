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
#include "system/debug.h"
#include "system/debug/draw_hitbox.h"
#include "system/debug/draw_path.h"
#include "system/debug/draw_position.h"
#include "system/debug/draw_target.h"
#include "system/event_messaging_sys.h"

#include "entity_factory.h"

static void on_player_enter_portal(void* arg, const EnterPortalMsg* event);
static void on_entity_died(void* arg, const EntityDiedMsg* event);
static void unload_current_level(void);
static void update_ui(void);
static void render_ui(void);
static void update_game_logic(void);
static void render_game_world(void);
static void load_pending_level(void);

void request_load_level(const char* level, const char* destination_portal);

static void music_player_on_level_loaded(void* arg, const LevelLoadedMsg* event);

DEFINE_SCENE(game);

Ecs* gEcs;

static BOOL _bHasNextLevel;
static char _pendingLevel[LADDER_ATTRS_MAX_LEVEL_NAME_LEN + 1];
static char _targetPortal[LADDER_ATTRS_MAX_DEST_LEN + 1];
static BOOL _isPaused;
static BOOL _isPlayerDied;

#if DEBUG
static BOOL _drawTileColliderEnabled;
static BOOL _drawHitboxEnable;
static BOOL _drawPositionEnabled;
static BOOL _drawRTreeEnabled;
static BOOL _drawGirdEnabled;
static BOOL _drawPathEnabled;
#endif

static void spawn_player(Vec2 position)
{
  ecs_entity_t player;

  player = make_character(gEcs, gSession.job, position);
  player = make_player(gEcs, player);

  ett_equip_weapon(gEcs, player, make_weapon(gEcs, gSession.weapon));
  ett_attune_spell(gEcs, player, gSession.spell);
}

static void remove_unprocessed_attack_commands(void)
{
  ecs_entity_t*  entities;
  ecs_size_t     cnt;
  AttackCommand* attackCommand;
  ecs_raw(gEcs, ATTACK_COMMAND, &entities, (void**)&attackCommand, &cnt);
  for (int i = cnt - 1; i >= 0; --i)
  {
    if (!attackCommand[i].processing)
    {
      INVOKE_EVENT(attackCommand[i].cbCompleted, FALSE);
      ecs_rmv(gEcs, entities[i], ATTACK_COMMAND);
    }
  }
}

static void on_load()
{
  extern Cursor g_cursor_cross;
  push_cursor_state(g_cursor_cross);
  gEcs = ecs_new(gCompDescs, NUM_COMPONENTS);

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
  pathfinding_system_init();

  // init ui
  ui_dialogue_init();
  ui_subtile_init();
  ui_msgbox_w_icon_init();

  inventory_init();

  ems_connect(MSG_HIT_LADDER, CALLBACK_2(on_player_enter_portal));
  ems_connect(MSG_ENTITY_DIED, CALLBACK_2(on_entity_died));
  ems_connect(MSG_LEVEL_LOADED, CALLBACK_2(music_player_on_level_loaded));

  input_push_state(INPUT_STATE_INST_1(player_process_input));

  inventory_load();
  load_level(gSession.level);
  ems_broadcast(MSG_LEVEL_LOADED, &(LevelLoadedMsg){ gSession.level });
  spawn_player(gSession.pos);
}

static void on_unload()
{
  inventory_save();
  ems_broadcast(MSG_GAME_SCENE_UNLOAD, NULL);
  dialogue_system_fini();
  game_event_fini();
  ecs_del(gEcs);
  ems_fini();

  gEcs = NULL;
}

static void on_event(const SDL_UNUSED SDL_Event* evt)
{
}

#if DEBUG
static void find_path_from_player_to_mouse_position(void)
{
  int         tileX, tileY, mouseX, mouseY;
  int         playerTileX, playerTileY;
  extern RECT gViewport;
  SDL_GetMouseState(&mouseX, &mouseY);
  mouseX = mouseX / SCL_X;
  mouseY = mouseY / SCL_Y;
  tileX  = (mouseX + gViewport.x) / TILE_SIZE;
  tileY  = (mouseY + gViewport.y) / TILE_SIZE;

  ecs_entity_t player         = scn_get_player(gEcs);
  Vec2         playerPosition = ett_get_position(gEcs, player);
  playerTileX                 = (int)playerPosition.x / TILE_SIZE;
  playerTileY                 = (int)playerPosition.y / TILE_SIZE;

  ecs_rmv(gEcs, player, PATH);
  ecs_set(gEcs,
          player,
          PATHFINDING_PARAMS,
          &(PathfindingParams){
              .start = { playerTileX, playerTileY },
              .goal  = { tileX, tileY },
          });
}
#endif

static void on_update()
{
#if DEBUG
  if (key_just_pressed(SDL_SCANCODE_H))
    _drawHitboxEnable = !_drawHitboxEnable;
  if (key_just_pressed(SDL_SCANCODE_J))
    _drawPositionEnabled = !_drawPositionEnabled;
  if (key_just_pressed(SDL_SCANCODE_K))
    _drawTileColliderEnabled = !_drawTileColliderEnabled;
  if (key_just_pressed(SDL_SCANCODE_L))
    _drawRTreeEnabled = !_drawRTreeEnabled;

  if (key_just_pressed(SDL_SCANCODE_N))
    _drawGirdEnabled = !_drawGirdEnabled;

  if (key_just_pressed(SDL_SCANCODE_M))
    _drawPathEnabled = !_drawPathEnabled;

  if (mouse_button_just_pressed(SDL_BUTTON_LEFT))
  {
    find_path_from_player_to_mouse_position();
  }
#endif

  if (_bHasNextLevel)
  {
    unload_current_level();
    load_pending_level();
  }
  else
  {
    // update
    if (!_isPaused)
    {
      update_game_logic();
    }

    update_ui();
    render_game_world();
    render_ui();

#if DEBUG
    if (_drawHitboxEnable)
      hitbox_rendering_system_update();

    if (_drawPositionEnabled)
      position_rendering_system_update();

    if (_drawTileColliderEnabled)
      draw_map_colliders();

    if (_drawRTreeEnabled)
      collision_system_render_debug();

    if (_drawPathEnabled)
      draw_path_system();

    if (_drawGirdEnabled)
      draw_gird();
    draw_destination_system();
#endif

    // late update
    remove_unprocessed_attack_commands();
    late_destroying_system();
    life_span_system();
  }
}

static void on_player_enter_portal(SDL_UNUSED void* arg, const EnterPortalMsg* event)
{
  PortalAttributes* portalAttributes = ecs_get(gEcs, event->portal, LADDER_ATTRIBUTES);
  request_load_level(portalAttributes->level, portalAttributes->dest);
}

void request_load_level(const char* level, const char* portalAttributes)
{
  SDL_strlcpy(_pendingLevel, level, sizeof(_pendingLevel) - 1);
  SDL_strlcpy(_targetPortal, portalAttributes, sizeof(_targetPortal) - 1);
  _bHasNextLevel = TRUE;
}

static void on_entity_died(SDL_UNUSED void* arg, const EntityDiedMsg* event)
{
  if (ecs_has(gEcs, event->entity, PLAYER_TAG))
    _isPlayerDied = TRUE;
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

static void music_player_on_level_loaded(SDL_UNUSED void* arg, const LevelLoadedMsg* event)
{
  Mix_Music* mus = music_from_level_name(event->level_name);
  if (mus != NULL)
    Mix_PlayMusic(mus, -1);
}

static void unload_current_level()
{
  ems_broadcast(MSG_LEVEL_UNLOADED, &(LevelUnloadedMsg){ gSession.level });
  ecs_entity_t player = scn_get_player(gEcs);
  gSession.spell      = ett_get_attuned_spell_type(gEcs, player);
  gSession.weapon     = ett_get_equiped_weapon_type(gEcs, player);
  ecs_clear(gEcs);
  map_clear();
}

void game_scene_pause()
{
  _isPaused = TRUE;
  ems_broadcast(MSG_GAME_PAUSED, NULL);
}

void game_scene_resume()
{
  _isPaused = FALSE;
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
  RUN_SYSTEM(pathfinding_system);
  RUN_SYSTEM(stats_system);
  RUN_SYSTEM(motion_system);
  RUN_SYSTEM(tile_collision_system);
  RUN_SYSTEM(collision_system);
  RUN_SYSTEM(equipment_system);
  RUN_SYSTEM(character_controller_system);
  RUN_SYSTEM(animator_system);
  RUN_SYSTEM(ai_system);
  RUN_SYSTEM(walk_directly_toward_system);
  RUN_SYSTEM(invulnerable_timer_system);
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

static Vec2 get_spawn_localtion(ecs_entity_t portal)
{
  PortalAttributes* portalAttributes = ecs_get(gEcs, portal, LADDER_ATTRIBUTES);
  HitBox*           hixbox           = ecs_get(gEcs, portal, HITBOX);
  Vec2              position         = ett_get_position(gEcs, portal);
  switch (portalAttributes->exitDirection)
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

static void load_pending_level(void)
{
  ecs_entity_t portal;
  Vec2         spawn_localtion;
  load_level(_pendingLevel);

  if ((portal = scn_find_portal(gEcs, _targetPortal)) != ECS_NULL_ENT)
  {
    spawn_localtion = get_spawn_localtion(portal);
    spawn_player(spawn_localtion);
  }
  SDL_strlcpy(gSession.level, _pendingLevel, sizeof(gSession.level) - 1);
  ems_broadcast(MSG_LEVEL_LOADED, &(LevelLoadedMsg){ _pendingLevel });

  _bHasNextLevel = FALSE;
}
