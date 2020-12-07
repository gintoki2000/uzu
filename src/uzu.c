#include <SDL2/SDL_image.h>

#include <stdio.h>
#include <stdlib.h>

#include <toolbox/toolbox.h>

#include <components.h>
#include <entity_factory.h>
#include <generator.h>
#include <map.h>
#include <map_data.h>
#include <resources.h>

#include <ecs/ecs.h>

#include <engine/engine.h>
#include <engine/keyboard.h>

#include <system/action_execution_system.h>
#include <system/animator_system.h>
#include <system/collision_filter.h>
#include <system/collision_system.h>
#include <system/draw_system.h>
#include <system/drawing_heal_bar.h>
#include <system/drawing_hitbox_system.h>
#include <system/drop_system.h>
#include <system/health_system.h>
#include <system/late_destroying_system.h>
#include <system/life_span_system.h>
#include <system/map_collision_system.h>
#include <system/mediator.h>
#include <system/motion_system.h>
#include <system/camera_following_system.h>
#include <system/pickup_system.h>
#include <system/player_controller_system.h>
#include <system/sound_sytem.h>
#include <system/swinging_system.h>
#include <system/sync_eqm_system.h>
#include <system/weapon_dealing_damage_system.h>

#include <ui/health_bar.h>

#define WIN_WIDTH 480
#define WIN_HEIGHT 360
#define SCL_X 2.0
#define SCL_Y 2.0

static Ecs* _ecs;

SDL_Rect g_viewport;

static void on_game_quit(void* user_data);
static BOOL on_game_init(void* user_data);
static void on_game_fini(void* user_data);
static void on_game_quit(void* user_data);
static void on_game_loop(void* user_data, SDL_Renderer* renderer);

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

  /*
  map_set_data(MAP_LAYER_FLOOR, MAP_TEST_FLOOR, MAP_TEST_COL_CNT * MAP_TEST_ROW_CNT);
  map_set_data(MAP_LAYER_WALL, MAP_TEST_WALL, MAP_TEST_COL_CNT * MAP_TEST_ROW_CNT);
  map_set_data(MAP_LAYER_FRONT, MAP_TEST_FRONT, MAP_TEST_COL_CNT * MAP_TEST_ROW_CNT);
  map_set_size(MAP_TEST_COL_CNT, MAP_TEST_ROW_CNT);
  map_enable_layer(MAP_LAYER_FLOOR);
  map_enable_layer(MAP_LAYER_WALL);
  */


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
    [CHARACTER_ACTION] =
        (EcsType){
            .size = sizeof(CharacterAction),
        },
    [WEAPON_ACTION] =
        (EcsType){
            .size = sizeof(WeaponAction),
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
            .size = sizeof(HealBar),
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
    [SWINGABLE] =
        (EcsType){
            .size = sizeof(Swingable),
        },
    [CHARACTER_STATS] =
        (EcsType){
            .size = sizeof(CharacterStats),
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
  };

  _ecs = ecs_new(types, NUM_COMPONENTS);

  // ecs_entity_t player = make_knight(_ecs, make_anime_sword(_ecs));
  // ecs_entity_t player =
  //  make_knight(_ecs, make_golden_sword(_ecs, CATEGORY_PLAYER_WEAPON, BIT(CATEGORY_ENEMY)));
  // ecs_entity_t player = make_knight(_ecs, make_bow(_ecs));
  //

  srand(SDL_GetTicks());
  set_generator_param(GENERATOR_PARAM_WIDTH, 100);
  set_generator_param(GENERATOR_PARAM_HEIGHT, 100);
  set_generator_param(GENERATOR_PARAM_MAX_ROOM_SIZE, 15);
  set_generator_param(GENERATOR_PARAM_MIN_ROOM_SIZE, 6);
  generate_new_dungeon(_ecs);

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
  sound_system_init(_ecs);

  Mix_PlayMusic(get_bg_mus(rand() % NUM_BG_MUSICS), -1);

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
  PlayerControllerSystem(_ecs);
  ActionExecutionSystem(_ecs);
  MotionSystem(_ecs);
  CollisionSystem(_ecs);
  MapCollisionSystem(_ecs);
  SyncEqmSystem(_ecs);
  AnimatorSystem(_ecs);
  CameraFollowingSystem(_ecs);
  SwingingSystem(_ecs);
  map_draw_layer(MAP_LAYER_FLOOR, renderer);
  map_draw_layer(MAP_LAYER_WALL, renderer);
  DrawSystem(_ecs, renderer);
  map_draw_layer(MAP_LAYER_FRONT, renderer);
  ui_heath_bar_draw(_ecs, renderer);
  LifeSpanSystem(_ecs);
  // collision_system_draw_debug(renderer);
  draw_rooms(renderer, 2);
  draw_graph(renderer, 2);
  draw_tree(renderer, 2);
  DrawingHealBarSystem(_ecs, renderer);
  DrawingHitboxSystem(_ecs, renderer);
  HealthSystem(_ecs);
  LateDestroyingSystem(_ecs);
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
