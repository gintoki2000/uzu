#include "behaviour_tree.h"
#include <SDL2/SDL_image.h>

#include <stdio.h>
#include <stdlib.h>

#include <toolbox/toolbox.h>

#include <components.h>
#include <constances.h>
#include <entity_factory.h>
#include <generator.h>
#include <map.h>
#include <map_data.h>
#include <resources.h>

#include <ecs/ecs.h>

#include <engine/engine.h>
#include <engine/keyboard.h>

#include "system/animator_ctl_sys.h"
#include "system/animator_sys.h"
#include "system/camera_sys.h"
#include "system/collision_mgr_sys.h"
#include "system/collision_sys.h"
#include "system/dmg_sys.h"
#include "system/draw_sys.h"
#include "system/health_bar_drawing_sys.h"
#include "system/health_sys.h"
#include "system/late_destroying_sys.h"
#include "system/life_span_sys.h"
#include "system/motion_sys.h"
#include "system/pickup_sys.h"
#include "system/player_ctl_sys.h"
#include "system/tile_collision_sys.h"
#include <system/ai_system.h>
#include <system/drop_system.h>
#include <system/mediator.h>
#include <system/equipment_sys.h>

#include "system/debug/draw_hitbox.h"
#include "system/debug/draw_target.h"
#include <system/debug/draw_path.h>
#include <system/debug/draw_position.h>

#include <system/weapon_skill/charge.h>
#include <system/weapon_skill/swing.h>

#include <ui/health_bar.h>

static Ecs* _ecs;

SDL_Rect g_viewport;

static void on_game_quit(void* user_data);
static BOOL on_game_init(void* user_data);
static void on_game_fini(void* user_data);
static void on_game_quit(void* user_data);
static void on_game_loop(void* user_data, SDL_Renderer* renderer);

static void swpan_player()
{
  ecs_entity_t player;
  Transform*   tx;

  player = make_player(_ecs, make_knight(_ecs), make_cleaver(_ecs, BIT(CATEGORY_ENEMY)));

  tx        = ecs_get(_ecs, player, TRANSFORM);
  tx->pos.x = WIN_WIDTH / 2;
  tx->pos.y = WIN_HEIGHT / 2 + 50;
}

static void swpan_enemy()
{
  ecs_entity_t enemy, e2;
  Transform*   tx;

  enemy     = make_huge_demon(_ecs);
  tx        = ecs_get(_ecs, enemy, TRANSFORM);
  tx->pos.x = TILE_SIZE * 6;
  tx->pos.y = TILE_SIZE * 7;

  e2        = make_chort(_ecs);
  tx        = ecs_get(_ecs, e2, TRANSFORM);
  tx->pos.x = TILE_SIZE * 5;
  tx->pos.y = TILE_SIZE * 12;
}
static BOOL on_game_init(void* user_data)
{
  (void)user_data;
  /*load asserts*/
  IMG_Init(IMG_INIT_PNG);
  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
  {
    ERROR("open audio failed\n");
    return FALSE;
  }

  // Mix_AllocateChannels(10);

  if (!resources_load())
  {
    ERROR("load resources failed\n");
    return FALSE;
  }

  /*set scale*/
  SDL_RenderSetScale(engine_get_renderer(), 2.0, 2.0);

  g_viewport = (SDL_Rect){ 0, 0, WIN_WIDTH, WIN_HEIGHT };

  map_set_data(MAP_LAYER_FLOOR, MAP_TEST_FLOOR, MAP_TEST_COL_CNT * MAP_TEST_ROW_CNT);
  map_set_data(MAP_LAYER_WALL, MAP_TEST_WALL, MAP_TEST_COL_CNT * MAP_TEST_ROW_CNT);
  map_set_data(MAP_LAYER_FRONT, MAP_TEST_FRONT, MAP_TEST_COL_CNT * MAP_TEST_ROW_CNT);
  map_set_size(MAP_TEST_COL_CNT, MAP_TEST_ROW_CNT);
  map_enable_layer(MAP_LAYER_FLOOR);
  map_enable_layer(MAP_LAYER_WALL);

  /*init keyboard*/
  keybroad_init();

  /*init _ecs*/
  EcsType types[NUM_COMPONENTS] = {
    [VISUAL] =
        (EcsType){
            .size = sizeof(Visual),
        },
    [TRANSFORM] =
        (EcsType){
            .size = sizeof(Transform),
        },
    [ANIMATOR] =
        (EcsType){
            .size    = sizeof(Animator),
            .fini_fn = (ecs_comp_fini_fn_t)animator_fini,
        },
    [PLAYER_TAG] =
        (EcsType){
            .size = sizeof(PlayerTag),
        },
    [CONTROLLER] =
        (EcsType){
            .size = sizeof(Controller),
        },
    [EQUIPMENT] =
        (EcsType){
            .size = sizeof(Equipment),
        },
    [HEATH] =
        (EcsType){
            .size = sizeof(Heath),
        },
    [HITBOX] =
        (EcsType){
            .size = sizeof(HitBox),
        },
    [ENEMY_TAG] =
        (EcsType){
            .size = sizeof(EnemyTag),
        },
    [HEAL_BAR] =
        (EcsType){
            .size = sizeof(HealthBar),
        },
    [LIFE_SPAN] =
        (EcsType){
            .size = sizeof(LifeSpan),
        },
    [MOTION] =
        (EcsType){
            .size = sizeof(Motion),
        },
    [PROJECTILE] =
        (EcsType){
            .size = sizeof(Projectile),
        },
    [TAG_TO_BE_DESTROYED] =
        (EcsType){
            .size = sizeof(TagToBeDestroyed),
        },
    [WEAPON_CORE] =
        (EcsType){
            .size = sizeof(WeaponCore),
        },
    [DAMAGE_OUTPUT] =
        (EcsType){
            .size = sizeof(DamageOutput),
        },
    [WEAPON_SKILL_SWING] =
        (EcsType){
            .size = sizeof(wpskl_Swing),
        },
    [WEAPON_SKILL_CHARGE] =
        (EcsType){
            .size = sizeof(wpskl_Charge),
        },
    [DROP] =
        (EcsType){
            .size = sizeof(Drop),
        },
    [INVULNERABLE] =
        (EcsType){
            .size = sizeof(Invulnerable),
        },
    [CAMERA_TARGET_TAG] =
        (EcsType){
            .size = sizeof(CameraTargetTag),
        },
    [AI_AGENT] =
        (EcsType){
            .size    = sizeof(AIAgent),
            .fini_fn = (ecs_comp_fini_fn_t)ai_agent_fini,
        },
    [BTV_MOVE_DESTINATION] =
        (EcsType){
            .size = sizeof(btv_MoveDestination),
        },
    [BTV_PATH] =
        (EcsType){
            .size = sizeof(btv_Path),
        },
  };

  _ecs = ecs_new(types, NUM_COMPONENTS);

  // ecs_entity_t player = make_knight(_ecs, make_anime_sword(_ecs));
  // ecs_entity_t player =
  //  make_knight(_ecs, make_golden_sword(_ecs, CATEGORY_PLAYER_WEAPON, BIT(CATEGORY_ENEMY)));
  // ecs_entity_t player = make_knight(_ecs, make_bow(_ecs));
  //

  srand(SDL_GetTicks());
  /*
  set_generator_param(GENERATOR_PARAM_WIDTH, 100);
  set_generator_param(GENERATOR_PARAM_HEIGHT, 100);
  set_generator_param(GENERATOR_PARAM_MAX_ROOM_SIZE, 15);
  set_generator_param(GENERATOR_PARAM_MIN_ROOM_SIZE, 6);
  generate_new_dungeon(_ecs);
  */

  swpan_player();
  // for (int i = 0; i < 20; ++i)
  swpan_enemy();

  /*
  for (int i = 0; i < 150; ++i)
  {
    ecs_entity_t demon = make_huge_demon(_ecs);

    Transform* dtx = ecs_get(_ecs, demon, TRANSFORM);
    dtx->pos       = VEC2(rand() % (WIN_WIDTH - 50) + 25, rand() % (WIN_HEIGHT - 50) + 25);
  }
  */

  mediator_init();
  collision_system_init(_ecs);
  health_system_init(_ecs);
  weapon_dealing_damage_system_init(_ecs);
  collision_filter_system_init(_ecs);
  drop_system_init(_ecs);
  pickup_system_init(_ecs);

  Mix_PlayMusic(get_bg_mus(BG_MUS_CATCH_THE_MYSTERY), -1);

  return TRUE;
}

static void on_game_fini(void* user_data)
{
  (void)user_data;

  resources_unload();
  ecs_del(_ecs);
  collision_system_fini();
  mediator_fini();
  _ecs = NULL;
  IMG_Quit();
  Mix_Quit();
}

static void on_game_loop(void* user_data, SDL_Renderer* renderer)
{
  (void)user_data;
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  SDL_RenderClear(renderer);
  keybroad_update();
  sys_ai_update(_ecs);
  sys_player_controller_update(_ecs);
  sys_equipment_update(_ecs);
  sys_motion_update(_ecs);
  sys_collision_update(_ecs);
  sys_tile_collision_update(_ecs);
  sys_wpskl_swing_update(_ecs);
  sys_wpskl_charge_update(_ecs);
  sys_animator_controller_update(_ecs);
  sys_animator_update(_ecs);
  sys_camera_update(_ecs);
  sys_heath_update(_ecs);
  sys_health_bar_update(_ecs, renderer);
  map_draw_layer(MAP_LAYER_FLOOR, renderer);
  map_draw_layer(MAP_LAYER_WALL, renderer);
  sys_draw_update(_ecs, renderer);
  map_draw_layer(MAP_LAYER_FRONT, renderer);
  ui_heath_bar_draw(_ecs, renderer);
  sys_life_span_update(_ecs);
  dbsys_rtree_update(renderer);
  // draw_rooms(renderer, 2);
  // draw_graph(renderer, 2);
  // draw_tree(renderer, 2);
  dbsys_hitbox_update(_ecs, renderer);
  dbsys_path_update(_ecs, renderer);
  dbsys_mvto_target_update(_ecs, renderer);
  dbsys_draw_position_update(_ecs, renderer);
  sys_late_destroying_update(_ecs);
  SDL_RenderPresent(renderer);
}

static void game_on_event(void* user_data, const SDL_Event* e)
{
  (void)user_data;
  if (e->type == SDL_QUIT)
  {
    on_game_quit(user_data);
  }
}

static void on_game_quit(void* user_data)
{
  (void)user_data;
  engine_stop();
}
/***********************************************************/

static GameDelegate delegate = (GameDelegate){
  .init  = on_game_init,
  .fini  = on_game_fini,
  .loop  = on_game_loop,
  .event = game_on_event,
};

static GameSetting setting = (GameSetting){
  .frame_rate    = 50,
  .window_title  = "app",
  .window_width  = 480 * 2,
  .window_height = 320 * 2,
};

int main()
{
  engine_run(&delegate, &setting);
  return 0;
}
