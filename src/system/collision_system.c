#include "collision_system.h"
#include <components.h>
#include <toolbox/toolbox.h>

#define BUFF_SIZE 255

static Dispatcher*   _dispatcher;
static RTree*        _rtree;
static CollisionPair _pair_buff[BUFF_SIZE];
static u32           _pair_cnt;

static ecs_entity_t get_player(Ecs* ecs)
{
  ecs_entity_t* ett;
  ecs_size_t    cnt;
  ecs_data(ecs, PLAYER_TAG, &ett, NULL, &cnt);
  ASSERT(cnt == 1 && "there is no player");
  return ett[0];
}

static void on_component_remove(void* udata, const EcsComponentEvent* event)
{
  (void)udata;
  if (event->type == HITBOX)
  {
    HitBox* hitbox = event->component;
    if (hitbox->proxy_id != NULL_NODE)
    {
      rtree_destroy_proxy(_rtree, hitbox->proxy_id);
      INFO("destroy proxy_id: %d\n", hitbox->proxy_id);
    }
  }
}

static AABB* query_aabb(AABB* aabb, const HitBox* hitbox, const Transform* transform)
{
  Rect rect;

  rect_init_full(&rect,
                 transform->pos.x - hitbox->anchor.x,
                 transform->pos.y - hitbox->anchor.y,
                 hitbox->size.x,
                 hitbox->size.y,
                 hitbox->anchor.x,
                 hitbox->anchor.y,
                 transform->rot);
  return rect_get_aabb(&rect, aabb);
}

static void update_proxies(Ecs* ecs)
{
  ecs_entity_t* entites;
  ecs_size_t    cnt;

  Transform* transform;
  HitBox*    hitboxs;

  AABB aabb;

  ecs_data(ecs, HITBOX, &entites, (void**)&hitboxs, &cnt);
  for (int i = 0; i < cnt; ++i)
  {
    transform = ecs_get(ecs, entites[i], TRANSFORM);
    query_aabb(&aabb, &hitboxs[i], transform);
    if (hitboxs[i].proxy_id == NULL_NODE)
    {
      hitboxs[i].proxy_id = rtree_create_proxy(_rtree, (void*)entites[i], &aabb);
    }
    else
    {
      rtree_move_proxy(_rtree, hitboxs[i].proxy_id, &aabb, VEC2(0, 0));
    }
  }
}

typedef struct __capture01
{
  ecs_entity_t entity;
  int          proxy_id;
} __capture01;

static void __lamda01(__capture01* capture, int proxy_id)
{
  if (capture->proxy_id != proxy_id)
  {
    ecs_entity_t entity = (ecs_entity_t)rtree_get_user_data(_rtree, proxy_id);
    _pair_buff[_pair_cnt++] = (CollisionPair){
      MAX(capture->entity, entity),
      MIN(capture->entity, entity),
    };
  }
}

static int compr_pair(const CollisionPair* p1, const CollisionPair* p2)
{
  if (p1->e1 != p2->e1)
    return p1->e1 - p2->e1;
  else
    return p1->e2 - p2->e2;
}

static BOOL pair_eq(const CollisionPair* p1, CollisionPair* p2)
{
  return (p1->e1 == p2->e1) && (p1->e2 == p2->e2);
}

static int remove_duplicate_pairs(CollisionPair* pairs, int cnt)
{

  if (cnt == 0 || cnt == 1)
    return cnt;

  int j = 0;

  for (int i = 0; i < cnt - 1; i++)
    if (!pair_eq(&pairs[i], &pairs[i + 1]))
      pairs[j++] = pairs[i];

  pairs[j++] = pairs[cnt - 1];
  return j;
}

static void broad_phase(Ecs* ecs)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  HitBox*       hitbox;
  Transform*    transform;
  AABB          aabb;
  __capture01   capture;

  _pair_cnt = 0;
  ecs_data(ecs, HITBOX, &entities, (void**)&hitbox, &cnt);
  for (int i = 0; i < cnt; ++i)
  {
    transform = ecs_get(ecs, entities[i], TRANSFORM);
    query_aabb(&aabb, &hitbox[i], transform);
    capture.proxy_id = hitbox[i].proxy_id;
    capture.entity = entities[i];
    rtree_query(_rtree, &aabb, CALLBACK_1(&capture, __lamda01));
  }
}

static void narrow_phase(Ecs* ecs)
{
  (void)ecs;

  if (_pair_cnt == 0)
    return;
  INFO("BEFORE SORTING:\n");
  for (u32 i = 0; i < _pair_cnt; ++i)
  {
    INFO("Pair{e1:{ %2u| %2u }, e2:{ %2u | %2u } }\n",
         ECS_ENT_IDX(_pair_buff[i].e1),
         ECS_ENT_VER(_pair_buff[i].e1),
         ECS_ENT_IDX(_pair_buff[i].e2),
         ECS_ENT_VER(_pair_buff[i].e2));
  }

  qsort(_pair_buff, _pair_cnt, sizeof(CollisionPair), (__compar_fn_t)compr_pair);

  INFO("AFTER SORTING:\n");
  for (u32 i = 0; i < _pair_cnt; ++i)
  {
    INFO("Pair{e1:{ %2u| %2u }, e2:{ %2u | %2u } }\n",
         ECS_ENT_IDX(_pair_buff[i].e1),
         ECS_ENT_VER(_pair_buff[i].e1),
         ECS_ENT_IDX(_pair_buff[i].e2),
         ECS_ENT_VER(_pair_buff[i].e2));
  }
  _pair_cnt =  remove_duplicate_pairs(_pair_buff, _pair_cnt);

  INFO("AFTER REMOVING DUPLICATES:\n");
  for (u32 i = 0; i < _pair_cnt; ++i)
  {
    INFO("Pair{e1:{ %2u| %2u }, e2:{ %2u | %2u } }\n",
         ECS_ENT_IDX(_pair_buff[i].e1),
         ECS_ENT_VER(_pair_buff[i].e1),
         ECS_ENT_IDX(_pair_buff[i].e2),
         ECS_ENT_VER(_pair_buff[i].e2));
  }
}

void collision_system_init(Ecs* ecs)
{
  _dispatcher = dispatcher_new(NUM_COLLISION_SIGS);
  _rtree = rtree_new();
  ecs_connect(ecs, ECS_SIG_COMP_RMV, NULL, (sig_handler_fn_t)on_component_remove);
}

void collision_system_fini()
{
  dispatcher_destroy(_dispatcher);
  rtree_delete(_rtree);
  _rtree = NULL;
  _dispatcher = NULL;
}

void collision_system_connect(int signal, pointer_t user_data, sig_handler_fn_t handler)
{
  dispatcher_connect(_dispatcher, signal, user_data, handler);
}

void collision_system_draw_debug(SDL_Renderer* renderer) { rtree_draw(_rtree, renderer); }

void CollisionSystem(Ecs* ecs)
{
  update_proxies(ecs);
  broad_phase(ecs);
  narrow_phase(ecs);
}
