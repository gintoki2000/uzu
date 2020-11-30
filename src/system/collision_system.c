#include <common.h>
#include <components.h>
#include <stdio.h>
#include <system/collision_system.h>
#include <toolbox/aabb-tree.h>

#define MAX_BUF_SIZE 150

typedef struct
{
  int          proxy_id;
  ecs_entity_t ett;
} __capture01;

typedef struct
{
  int ia;
  int ib;
} CollisionPair;

static void  __lamda01(void* user_data, int id);
static AABB* aabb_form_hitbox(AABB* aabb, const HitBox* hb, const Transform* tx);

static AABBTree*     _tree;
static CollisionPair _pair_buff[MAX_BUF_SIZE];
static u32           _pair_buff_size;

void collision_system_init()
{
  _tree = aabb_tree_new();
  _pair_buff_size = 0;
}

void collision_system_fini()
{
  aabb_tree_delete(_tree);
  _tree = NULL;
}

void CollisionSystem(Ecs* ecs)
{
  ecs_entity_t* ett;
  HitBox*       hb;
  ecs_size_t    cnt;
  __capture01   capture;
  AABB          aabb;

  ecs_data(ecs, HITBOX, &ett, (void**)&hb, &cnt);

  _pair_buff_size = 0;
  for (int i = 0; i < cnt; ++i)
  {
    if (hb[i].proxy_id == -1)
    {
      // create new one
      aabb_form_hitbox(&aabb, &hb[i], ecs_get(ecs, ett[i], TRANSFORM));
      hb[i].proxy_id = aabb_tree_create_proxy(_tree, (void*)ett[i], &aabb);
    }
    else
    {
      aabb_form_hitbox(&aabb, &hb[i], ecs_get(ecs, ett[i], TRANSFORM));
      aabb_tree_move_proxy(_tree, hb[i].proxy_id, &aabb, (Vec2){ 10, 10 });
    }
  }

  for (int i = 0; i < cnt; ++i)
  {
    capture.proxy_id = hb[i].proxy_id;
    aabb_tree_query(_tree, &aabb, CALLBACK_1(&capture, __lamda01));
  }

  for (u32 i = 0; i < _pair_buff_size; ++i)
  {
    printf("Collision[e1 = %u, e2 = %u]\n",
           (u32)aabb_tree_get_user_data(_tree, _pair_buff[i].ia),
           (u32)aabb_tree_get_user_data(_tree, _pair_buff[i].ib));
  }
}

static void __lamda01(void* user_data, int pid)
{
  __capture01* vals;

  vals = (__capture01*)user_data;

  if (vals->proxy_id != pid)
  {
    _pair_buff[_pair_buff_size++] = (CollisionPair){
      max(vals->proxy_id, pid),
      min(vals->proxy_id, pid),
    };
  }
}

static void vec2_translate(Vec2* v, int x, int y)
{
  v->x += x;
  v->y += y;
}

static AABB* aabb_form_hitbox(AABB* aabb, const HitBox* hb, const Transform* tx)
{
  aabb->lower_bound.x = tx->x - hb->hwidth;
  aabb->lower_bound.y = tx->y - hb->hheight;
  aabb->upper_bound.x = tx->x + hb->hwidth;
  aabb->upper_bound.y = tx->y + hb->hheight;

  // vec2_translate(&aabb->upper_bound, -hb->anchor_x, -hb->anchor_y);
  // vec2_translate(&aabb->lower_bound, -hb->anchor_x, -hb->anchor_y);
  return aabb;
}
