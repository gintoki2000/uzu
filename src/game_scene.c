#include "game_scene.h"
#include "SDL_mixer.h"
#include "components.h"
#include "constances.h"
#include "ecs/ecs.h"
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
#include "ui_msgbox_w_icon.h"
#include "ui_quality.h"
#include "ui_subtile.h"

#include "engine/keyboard.h"

#include "system/collision_sys.h"
#include "system/game_logic.h"
#include "system/rendering.h"

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

#include "entity_utils.h"
#include "system/event_messaging_sys.h"

static void on_load(void);
static void on_unload(void);
static void on_event(const SDL_Event* evt);
static void on_update();
static void on_player_hit_ladder(pointer_t arg, const MSG_HitLadder* event);
static void on_entity_died(pointer_t arg, const MSG_EntityDied* event);
static void unload_current_level(void);
static void update_ui(void);
static void render_ui(void);
static void update_game_logic(void);
static void render_game_world(void);
static void next_level(void);

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
static BOOL _player_died;

static void spawn_player(Vec2 position)
{
  ecs_entity_t player;
  ecs_entity_t weapon;
  ecs_entity_t (*make_weapon_fn)(Ecs*, u16);
  ecs_entity_t (*make_character_fn)(Ecs*, Vec2);
  u16 mask = BIT(CATEGORY_ENEMY) | BIT(CATEGORY_INTERACABLE);

  make_weapon_fn    = g_weapon_create_fn_tbl[g_session.weapon];
  make_character_fn = g_char_create_fn_tbl[g_session.job];

  player = make_character_fn(g_ecs, position);
  weapon = make_weapon_fn(g_ecs, mask);

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
  effect_system_init();

  // init ui
  ui_dialogue_init();
  ui_subtile_init();
  ui_msgbox_w_icon_init();

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

#if 0
    // render debug
    collision_system_render_debug();
    hitbox_rendering_system_update();
    position_rendering_system_update();
#endif

    // late update
    late_destroying_system_update();
    life_span_system_update();
  }
}

static void on_player_hit_ladder(SDL_UNUSED pointer_t arg, const MSG_HitLadder* event)
{
  LadderAttributes* attrs = ecs_get(g_ecs, event->ladder, LADDER_ATTRIBUTES);
  strcpy(_next_level, attrs->level);
  strcpy(_spwan_location, attrs->dest);
  _has_next_level = TRUE;
}

static void on_entity_died(SDL_UNUSED pointer_t arg, SDL_UNUSED const MSG_EntityDied* event)
{
  if (ecs_has(g_ecs, event->entity, PLAYER_TAG))
    _player_died = TRUE;
}

typedef struct MusTblItem
{
  const char* level_name;
  u16         mus_id;
} MusTblItem;

static MusTblItem _mustbl[] = { { "0", BG_MUS_LV1 }, { "1", BG_MUS_LV2 }, { NULL, 0 } };

static Mix_Music* get_bgmusic_by_level_name(const char* level_name)
{
  for (int i = 0; _mustbl[i].level_name != NULL; ++i)
  {
    if (STREQ(level_name, _mustbl[i].level_name))
      return get_bg_mus(_mustbl[i].mus_id);
  }
  return NULL;
}

static void music_player_on_level_loaded(SDL_UNUSED void* arg, const MSG_LevelLoaded* event)
{
  Mix_Music* mus = get_bgmusic_by_level_name(event->level_name);
  if (mus != NULL)
    Mix_PlayMusic(mus, -1);
}

static void __callback_clear_entities(SDL_UNUSED pointer_t arg, Ecs* ecs, ecs_entity_t entity)
{
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
  weapon_skill_thust_update();
  thunder_storm_weapon_skl_system_update();
  // charge_weapon_skl_system();
}

static void render_game_world(void)
{
  map_draw(MAP_LAYER_FLOOR);
  map_draw(MAP_LAYER_BACK_WALL);
  rendering_system_update();
  healthbar_rendering_system_update();
  map_draw(MAP_LAYER_FRONT_WALL);
  text_rendering_system_update();
  interactable_pointer_rendering_system_update();
  hub_system_update();
  dialogue_system_update();
  merchant_system_update();
}

static Vec2 get_spawn_localtion(ecs_entity_t ladder)
{

  Vec2 position = get_entity_position(g_ecs, ladder);
  position.x += 8;
  position.y += 30;
  return position;
}

static void next_level(void)
{
  ecs_entity_t ladder;
  Vec2         spawn_localtion;
  load_level(_next_level);

  if ((ladder = find_ladder(g_ecs, _spwan_location)) != ECS_NULL_ENT)
  {
    spawn_localtion = get_spawn_localtion(ladder);
    spawn_player(spawn_localtion);
  }
  ems_broadcast(MSG_LEVEL_LOADED, &(MSG_LevelLoaded){ _next_level });

  _has_next_level = FALSE;
}
