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
#include <system/draw_system.h>
#include <system/generic_axe_system.h>
#include <system/movement_system.h>
#include <system/player_controller_system.h>
#include <system/sword_system.h>
#include <system/sync_eqm_system.h>

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
    ERROR("load resources failed");
    return FALSE;
  }

  /*set scale*/
  SDL_RenderSetScale(engine_get_renderer(), 2.0, 2.0);

  /*init keyboard*/
  keybroad_init();

  /*init _ecs*/
  EcsType types[NUM_COMPONENTS] = {
    [VISUAL] = (EcsType){ .size = sizeof(Visual) },
    [TRANSFORM] = (EcsType){ .size = sizeof(Transform) },
    [ANIMATOR] =
        (EcsType){
            .size = sizeof(Animator),
            .fini_fn = (ecs_comp_fini_fn_t)animator_fini,
        },
    [PLAYER_TAG] = (EcsType){ .size = sizeof(PlayerTag) },
    [COMMAND_INPUT] = (EcsType){ .size = sizeof(CmdInput) },
    [WP_CMD_INPUT] = (EcsType){ .size = sizeof(WeaponCmdInput) },
    [GENERIC_SWORD] = (EcsType){ .size = sizeof(GenericSword) },
    [EQUIPMENT] = (EcsType){ .size = sizeof(Equipment) },
    [GENERIC_AXE] = (EcsType){ .size = sizeof(GenericAxe) },
    [HEATH] = (EcsType){ .size = sizeof(Heath) },
    [HITBOX] = (EcsType){ .size = sizeof(HitBox) },
  };

  _ecs = ecs_new(types, NUM_COMPONENTS);

  ecs_entity_t player = ettfactory_make_knight(_ecs, ettfactory_make_anime_sword(_ecs));

  ecs_add(_ecs, player, PLAYER_TAG);

  Transform* tx = ecs_get(_ecs, player, TRANSFORM);
  tx->x = WIN_WIDTH / 2;
  tx->y = WIN_HEIGHT / 2;

  ecs_entity_t demon = ettfactory_make_huge_demon(_ecs, ettfactory_make_axe(_ecs));

  Transform* dtx = ecs_get(_ecs, demon, TRANSFORM);
  dtx->x = WIN_WIDTH / 2 + 100;
  dtx->y = WIN_HEIGHT / 2;

  return TRUE;
}

static void on_game_fini(void* user_data)
{
  (void)user_data;
  resources_unload();
  ecs_del(_ecs);
  IMG_Quit();
}

static void on_game_loop(void* user_data, SDL_Renderer* renderer)
{
  (void)user_data;
  SDL_RenderClear(renderer);
  keybroad_update();
  PlayerControllerSystem(_ecs);
  MovementSystem(_ecs);
  SyncEqmSystem(_ecs);
  AnimatorSystem(_ecs);
  DrawSystem(_ecs, renderer);
  GenericSwordSystem(_ecs);
  GenericAxeSystem(_ecs);
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
