#include "collision_system.h"
#include <components.h>
#include <toolbox/toolbox.h>

#define BUFF_SIZE 100

static CollisionPair _pve_pairs[BUFF_SIZE];
static int           _pve_pair_cnt;
static CollisionPair _wpve_pairs[BUFF_SIZE];
static int           _wpve_pair_cnt;
static Dispatcher*   _dispatcher;

static ecs_entity_t get_player(Ecs* ecs)
{
  ecs_entity_t* ett;
  ecs_size_t    cnt;
  ecs_data(ecs, PLAYER_TAG, &ett, NULL, &cnt);
  ASSERT(cnt == 1 && "there is no player");
  return ett[0];
}

static Rect* query_boundingbox(Ecs* ecs, ecs_entity_t ett, Rect* rect)
{
  HitBox*    hitbox;
  Transform* transform;
  float      x, y, w, h;

  hitbox = ecs_get(ecs, ett, HITBOX);
  transform = ecs_get(ecs, ett, TRANSFORM);

  x = transform->x - hitbox->anchor.x;
  y = transform->y - hitbox->anchor.y;
  w = hitbox->size.x;
  h = hitbox->size.y;
  rect_init_full(rect, x, y, w, h, hitbox->anchor.x, hitbox->anchor.y, transform->rot);
  return rect;
}

static void put_pve_pair(ecs_entity_t player, ecs_entity_t enemy)
{
  _pve_pairs[_pve_pair_cnt++] = (CollisionPair){ player, enemy };
}

static void notify()
{
  for (int i = 0; i < _pve_pair_cnt; ++i)
  {
    dispatcher_emit(_dispatcher, SIG_PLAYER_COLLIDED_W_ENEMY, &_pve_pairs[i]);
  }

  for (int i = 0; i < _wpve_pair_cnt; ++i)
  {
    dispatcher_emit(_dispatcher, SIG_PLAYER_WEAPON_COLLIED_W_ENEMY, &_wpve_pairs[i]);
  }
}

void collision_system_init() { _dispatcher = dispatcher_new(NUM_COLLISION_SIGS); }

void collision_system_fini()
{
  dispatcher_destroy(_dispatcher);
  _dispatcher = NULL;
}

void collision_system_connect(int signal, pointer_t user_data, sig_handler_fn_t handler)
{
  dispatcher_connect(_dispatcher, signal, user_data, handler);
}

static void check_player_weapon_w_enemy(Ecs* ecs)
{
  ecs_entity_t* enemies;
  ecs_size_t    enemy_cnt;
  ecs_entity_t* weapons;
  ecs_size_t    weapon_cnt;
  Rect          player_weapon_hitbox;
  Rect          enemy_hitbox;

  ecs_data(ecs, PLAYER_WEAPON_TAG, &weapons, NULL, &weapon_cnt);
  ecs_data(ecs, ENEMY_TAG, &enemies, NULL, &enemy_cnt);

  _wpve_pair_cnt = 0;
  for (int wi = 0; wi < weapon_cnt; ++wi)
  {
    query_boundingbox(ecs, weapons[wi], &player_weapon_hitbox);
    for (int ei = 0; ei < enemy_cnt; ++ei)
    {
      query_boundingbox(ecs, enemies[ei], &enemy_hitbox);
      if (rect_has_intersection(&player_weapon_hitbox, &enemy_hitbox))
      {
        _wpve_pairs[_wpve_pair_cnt++] = (CollisionPair){ weapons[wi], enemies[ei] };
      }
    }
  }
}

static void check_player_w_enemy(Ecs* ecs)
{
  ecs_entity_t* enemies;
  ecs_size_t    enemy_cnt;
  ecs_entity_t  player;
  Rect          player_hitbox;
  Rect          enemy_hitbox;

  player = get_player(ecs);
  query_boundingbox(ecs, player, &player_hitbox);

  ecs_data(ecs, ENEMY_TAG, &enemies, NULL, &enemy_cnt);

  _pve_pair_cnt = 0;
  for (int i = 0; i < enemy_cnt; ++i)
  {
    query_boundingbox(ecs, enemies[i], &enemy_hitbox);
    if (rect_has_intersection(&player_hitbox, &enemy_hitbox))
    {
      put_pve_pair(player, enemies[i]);
    }
  }
}

void CollisionSystem(Ecs* ecs)
{
  check_player_w_enemy(ecs);
  check_player_weapon_w_enemy(ecs);
  notify();
}
