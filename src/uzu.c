#include <SDL2/SDL_image.h>

#include <stdio.h>
#include <stdlib.h>

#include <toolbox/toolbox.h>

#include <components.h>
#include <entity_factory.h>
#include <resources.h>

#include <ecs/ecs.h>

#include <engine/engine.h>
#include <engine/keyboard.h>

#include <system/animator_system.h>
#include <system/collision_system.h>
#include <system/draw_system.h>
#include <system/drawing_heal_bar.h>
#include <system/drawing_hitbox_system.h>
#include <system/generic_axe_system.h>
#include <system/generic_bow_system.h>
#include <system/health_system.h>
#include <system/life_span_system.h>
#include <system/movement_system.h>
#include <system/player_controller_system.h>
#include <system/sword_system.h>
#include <system/sync_eqm_system.h>
#include <system/motion_system.h>

#define WIN_WIDTH 480
#define WIN_HEIGHT 360
#define SCL_X 2.0
#define SCL_Y 2.0

static Ecs* _ecs;

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
  if (!resources_load())
  {
    ERROR("load  failed");
    return FALSE;
  }

  /*set scale*/
  SDL_RenderSetScale(engine_get_renderer(), 2.0, 2.0);

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
            .size = sizeof(Animator),
            .fini_fn = (ecs_comp_fini_fn_t)animator_fini,
        },
    [PLAYER_TAG] =
        (EcsType){
            .size = sizeof(PlayerTag),
        },
    [COMMAND_INPUT] =
        (EcsType){
            .size = sizeof(CmdInput),
        },
    [WP_CMD_INPUT] =
        (EcsType){
            .size = sizeof(WeaponCmdInput),
        },
    [GENERIC_SWORD] =
        (EcsType){
            .size = sizeof(GenericSword),
        },
    [EQUIPMENT] =
        (EcsType){
            .size = sizeof(Equipment),
        },
    [GENERIC_AXE] =
        (EcsType){
            .size = sizeof(GenericAxe),
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
    [PLAYER_WEAPON_TAG] =
        (EcsType){
            .size = sizeof(PlayerWeaponTag),
        },
    [HEAL_BAR] =
        (EcsType){
            .size = sizeof(HealBar),
        },
    [LIFE_SPAN] =
        (EcsType){
            .size = sizeof(LifeSpan),
        },
    [GENERIC_BOW] =
        (EcsType){
            .size = sizeof(GenericBow),
        },
    [MOTION] =
        (EcsType){
            .size = sizeof(Motion),
        },
    [PROJECTILE] =
        (EcsType){
            .size = sizeof(Projectile),
        },
  };

  _ecs = ecs_new(types, NUM_COMPONENTS);


  // ecs_entity_t player = make_knight(_ecs, make_anime_sword(_ecs));

  ecs_entity_t player = make_knight(_ecs, make_bow(_ecs));

  ecs_add(_ecs, player, PLAYER_TAG);

  Transform* tx = ecs_get(_ecs, player, TRANSFORM);
  tx->pos.x = WIN_WIDTH / 2;
  tx->pos.y = WIN_HEIGHT / 2;

  srand(SDL_GetTicks());
  for (int i = 0; i < 5; ++i)
  {
    ecs_entity_t demon = make_huge_demon(_ecs, ECS_NULL_ENT);

    Transform* dtx = ecs_get(_ecs, demon, TRANSFORM);
    dtx->pos.x = rand() % WIN_WIDTH;
    dtx->pos.y = rand() % WIN_HEIGHT;
  }

  collision_system_init(_ecs);
  health_system_init();
  generic_sword_system_init(_ecs);
  generic_bow_system_init(_ecs);



  return TRUE;
}

static void on_game_fini(void* user_data)
{
  (void)user_data;

  generic_sword_system_fini();
  health_system_fini();
  collision_system_fini();
  resources_unload();
  ecs_del(_ecs);
  _ecs = NULL;
  IMG_Quit();
}

static void on_game_loop(void* user_data, SDL_Renderer* renderer)
{
  (void)user_data;
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  SDL_RenderClear(renderer);
  keybroad_update();
  PlayerControllerSystem(_ecs);
  MovementSystem(_ecs);
  MotionSystem(_ecs); 
  SyncEqmSystem(_ecs);
  CollisionSystem(_ecs);
  AnimatorSystem(_ecs);
  DrawSystem(_ecs, renderer);
  LifeSpanSystem(_ecs);
  collision_system_draw_debug(renderer);
  //DrawingHealBarSystem(_ecs, renderer);
  DrawingHitboxSystem(_ecs, renderer);
  GenericSwordSystem(_ecs);
  GenericAxeSystem(_ecs);
  GenericBowSystem(_ecs);
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
  .init = on_game_init,
  .fini = on_game_fini,
  .loop = on_game_loop,
  .event = game_on_event,
};

static GameSetting setting = (GameSetting){
  .frame_rate = 30,
  .window_title = "app",
  .window_width = 480 * 2,
  .window_height = 320 * 2,
};

int main()
{
  engine_run(&delegate, &setting);
  return 0;
}
