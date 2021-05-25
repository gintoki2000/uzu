#include "components.h"
#include "game_scene.h"
#include "system/collision_sys.h"
#include "system/event_messaging_sys.h"
#include "toolbox/toolbox.h"

#define BUFF_SIZE 300

static RTree*        _rtree;
static CollisionPair _pairBuff[BUFF_SIZE];
static u32           _pairCnt;

extern SDL_Rect      gViewport;
extern SDL_Renderer* gRenderer;
extern Ecs*          gEcs;

static void on_hitbox_remove(SDL_UNUSED void* arg, const EcsComponentEvent* event)
{
  HitBox* hitbox = event->component;
  if (hitbox->proxyId != RTREE_NULL_NODE)
    rtree_destroy_proxy(_rtree, hitbox->proxyId);
}

INLINE RECT* calculate_bounding_rect(RECT* r, const HitBox* hitbox, const Transform* transform)
{
  r->w = hitbox->size.x;
  r->h = hitbox->size.y;
  r->x = transform->position.x - hitbox->anchor.x;
  r->y = transform->position.y - hitbox->anchor.y;
  return r;
}

INLINE AABB* get_aabb(AABB* aabb, const HitBox* hitbox, const Transform* transform)
{
  aabb->lower_bound.x = transform->position.x - hitbox->anchor.x;
  aabb->lower_bound.y = transform->position.y - hitbox->anchor.y;
  aabb->upper_bound.x = aabb->lower_bound.x + hitbox->size.x;
  aabb->upper_bound.y = aabb->lower_bound.y + hitbox->size.y;
  return aabb;
}

static void update_proxies(Ecs* ecs)
{
  ecs_entity_t* entites;
  ecs_size_t    cnt;

  Transform* transform;
  HitBox*    hitboxs;

  AABB  aabb;
  float dx, dy;

  ecs_raw(ecs, HITBOX, &entites, (void**)&hitboxs, &cnt);
  for (int i = 0; i < cnt; ++i)
  {
    transform = ecs_get(ecs, entites[i], TRANSFORM);
    if (hitboxs[i].proxyId == RTREE_NULL_NODE)
    {
      get_aabb(&aabb, &hitboxs[i], transform);
      hitboxs[i].proxyId = rtree_create_proxy(_rtree, (void*)entites[i], &aabb);
    }
    else
    {
      dx = transform->position.x - transform->lastPosition.x;
      dy = transform->position.y - transform->lastPosition.y;
      if (absf(dx) > EPSILON || absf(dy) > EPSILON)
      {
        get_aabb(&aabb, &hitboxs[i], transform);
        rtree_move_proxy(_rtree, hitboxs[i].proxyId, &aabb, (Vec2){ 0, 0 });
      }
    }
  }
}

typedef struct
{
  ecs_entity_t entity;
  int          proxy_id;
} CBBroadPhaseQueryVars;

INLINE ecs_entity_t maxe(ecs_entity_t e1, ecs_entity_t e2)
{
  return e1 > e2 ? e1 : e2;
}

INLINE ecs_entity_t mine(ecs_entity_t e1, ecs_entity_t e2)
{
  return e1 < e2 ? e1 : e2;
}

static BOOL cb_broard_phase_query(CBBroadPhaseQueryVars* vars, int proxy_id)
{
  if (vars->proxy_id != proxy_id)
  {
    ASSERT(_pairCnt < BUFF_SIZE);
    ecs_entity_t entity     = (ecs_entity_t)rtree_get_user_data(_rtree, proxy_id);
    _pairBuff[_pairCnt++] = (CollisionPair){
      maxe(vars->entity, entity),
      mine(vars->entity, entity),
    };
  }
  return TRUE;
}

static int compr_collision_pair(const CollisionPair* p1, const CollisionPair* p2)
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
  ecs_entity_t*         entities;
  ecs_size_t            cnt;
  HitBox*               hitbox;
  Transform*            transform;
  AABB                  aabb;
  CBBroadPhaseQueryVars vars;

  _pairCnt = 0;
  ecs_raw(ecs, HITBOX, &entities, (void**)&hitbox, &cnt);
  for (int i = 0; i < cnt; ++i)
  {
    transform = ecs_get(ecs, entities[i], TRANSFORM);
    get_aabb(&aabb, &hitbox[i], transform);
    vars.proxy_id = hitbox[i].proxyId;
    vars.entity   = entities[i];
    rtree_query(_rtree, &aabb, CALLBACK_1(&vars, cb_broard_phase_query));
  }
}

static void narrow_phase(Ecs* ecs)
{

  if (_pairCnt == 0)
    return;

  RECT       r1, r2;
  HitBox *   hitbox1, *hitbox2;
  Transform *transform1, *transform2;

  SDL_qsort(_pairBuff, _pairCnt, sizeof(CollisionPair), (__compar_fn_t)compr_collision_pair);

  _pairCnt = remove_duplicate_pairs(_pairBuff, _pairCnt);

  for (u32 i = 0; i < _pairCnt; ++i)
  {
    hitbox1 = ecs_get(ecs, _pairBuff[i].e1, HITBOX);
    hitbox2 = ecs_get(ecs, _pairBuff[i].e2, HITBOX);
    if ((BIT(hitbox1->category) & hitbox2->mask) && (BIT(hitbox2->category) & hitbox1->mask))
    {
      transform1 = ecs_get(gEcs, _pairBuff[i].e1, TRANSFORM);
      transform2 = ecs_get(gEcs, _pairBuff[i].e2, TRANSFORM);
      calculate_bounding_rect(&r1, hitbox1, transform1);
      calculate_bounding_rect(&r2, hitbox2, transform2);
      if (SDL_HasIntersection(&r1, &r2))
      {
        ems_broadcast(MSG_COLLISION,
                      &(OverlapMsg){
                          _pairBuff[i].e1,
                          _pairBuff[i].e2,
                      });
      }
    }
  }
}

void collision_system_init()
{
  _rtree = rtree_new();
  signal_connect(ecs_on_destruct(gEcs, HITBOX), CALLBACK_2(on_hitbox_remove));
}

void collision_system_fini()
{
  rtree_delete(_rtree);
  _rtree = NULL;
}

void collision_system_render_debug()
{
  rtree_draw_debug(_rtree, gRenderer, &gViewport);
}

void query_box_collision(const AABB* aabb, Callback callback)
{
  rtree_query(_rtree, aabb, callback);
}

void collision_system()
{
  update_proxies(gEcs);
  broad_phase(gEcs);
  narrow_phase(gEcs);
}

typedef struct
{
  // input
  const RECT* rect;
  u16         mask_bits;
  u16         max;

  // output
  Callback callback;
} CBCollision_BoxQueryArgs;

static BOOL __callback_box_query(CBCollision_BoxQueryArgs* vars, int proxy_id)
{
  ecs_entity_t entity;
  Transform*   transform;
  HitBox*      hitbox;
  RECT         boundingRect;

  entity = (ecs_entity_t)rtree_get_user_data(_rtree, proxy_id);

  transform = ecs_get(gEcs, entity, TRANSFORM);
  hitbox    = ecs_get(gEcs, entity, HITBOX);

  if (BIT(hitbox->category) & vars->mask_bits)
  {
    calculate_bounding_rect(&boundingRect, hitbox, transform);
    if (SDL_HasIntersection(vars->rect, &boundingRect))
    {
      return INVOKE_CALLBACK(vars->callback, BOOL, entity);
    }
  }
  return TRUE;
}

void collision_box_query(const RECT* rect, u16 maskBits, Callback callback)
{
  AABB aabb;

  aabb.lower_bound.x = rect->x;
  aabb.lower_bound.y = rect->y;
  aabb.upper_bound.x = rect->x + rect->w;
  aabb.upper_bound.y = rect->y + rect->h;

  rtree_query(_rtree,
              &aabb,
              CALLBACK_1((&(CBCollision_BoxQueryArgs){
                             .rect      = rect,
                             .mask_bits = maskBits,
                             .callback  = callback,
                         }),
                         __callback_box_query));
}
