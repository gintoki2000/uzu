#include "collision_sys.h"
#include <components.h>
#include <game_scene.h>
#include <toolbox/toolbox.h>
#include "event_messaging_sys.h"

#define BUFF_SIZE 300

static RTree*        _rtree;
static CollisionPair _pair_buff[BUFF_SIZE];
static u32           _pair_cnt;

extern SDL_Rect      g_viewport;
extern SDL_Renderer* g_renderer;
extern Ecs*          g_ecs;

static void on_component_remove(void* arg, const EcsComponentEvent* event)
{
  (void)arg;
  if (event->type == HITBOX)
  {
    HitBox* hitbox = event->component;
    if (hitbox->proxy_id != RTREE_NULL_NODE)
    {
      rtree_destroy_proxy(_rtree, hitbox->proxy_id);
    }
  }
}

static Rect* get_bounding_rect(Rect* r, const HitBox* hitbox, const Transform* transform)
{
  rect_init_full(r,
                 transform->pos.x - hitbox->anchor.x,
                 transform->pos.y - hitbox->anchor.y,
                 hitbox->size.x,
                 hitbox->size.y,
                 hitbox->anchor.x,
                 hitbox->anchor.y,
                 transform->rot);
  return r;
}

static AABB* get_aabb(AABB* aabb, const HitBox* hitbox, const Transform* transform)
{
  Rect rect;
  get_bounding_rect(&rect, hitbox, transform);
  return rect_get_aabb(&rect, aabb);
}

static void update_proxies(Ecs* ecs)
{
  ecs_entity_t* entites;
  ecs_size_t    cnt;

  Transform* transform;
  HitBox*    hitboxs;

  AABB aabb;

  ecs_raw(ecs, HITBOX, &entites, (void**)&hitboxs, &cnt);
  for (int i = 0; i < cnt; ++i)
  {
    transform = ecs_get(ecs, entites[i], TRANSFORM);
    if (hitboxs[i].proxy_id == RTREE_NULL_NODE)
    {
      get_aabb(&aabb, &hitboxs[i], transform);
      hitboxs[i].proxy_id = rtree_create_proxy(_rtree, (void*)entites[i], &aabb);
    }
    else
    {
      float dx, dy, dr;
      dx = transform->pos.x - transform->prev_pos.x;
      dy = transform->pos.y - transform->prev_pos.y;
      dr = transform->rot - transform->prev_rot;
      if (absf(dx) > EPSILON || absf(dy) > EPSILON || absf(dr) > EPSILON)
      {
        get_aabb(&aabb, &hitboxs[i], transform);
        rtree_move_proxy(_rtree, hitboxs[i].proxy_id, &aabb, VEC2(0, 0));
      }
    }
  }
}

typedef struct
{
  ecs_entity_t entity;
  int          proxy_id;
} CBBroadPhaseQueryVars;

static BOOL cb_broard_phase_query(CBBroadPhaseQueryVars* vars, int proxy_id)
{
  if (vars->proxy_id != proxy_id)
  {
    ASSERT(_pair_cnt < BUFF_SIZE);
    ecs_entity_t entity     = (ecs_entity_t)rtree_get_user_data(_rtree, proxy_id);
    _pair_buff[_pair_cnt++] = (CollisionPair){
      MAX(vars->entity, entity),
      MIN(vars->entity, entity),
    };
  }
  return TRUE;
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
  ecs_entity_t*         entities;
  ecs_size_t            cnt;
  HitBox*               hitbox;
  Transform*            transform;
  AABB                  aabb;
  CBBroadPhaseQueryVars vars;

  _pair_cnt = 0;
  ecs_raw(ecs, HITBOX, &entities, (void**)&hitbox, &cnt);
  for (int i = 0; i < cnt; ++i)
  {
    transform = ecs_get(ecs, entities[i], TRANSFORM);
    get_aabb(&aabb, &hitbox[i], transform);
    vars.proxy_id = hitbox[i].proxy_id;
    vars.entity   = entities[i];
    rtree_query(_rtree, &aabb, CALLBACK_1(&vars, cb_broard_phase_query));
  }
}

static void narrow_phase(Ecs* ecs)
{

  if (_pair_cnt == 0)
    return;

  Rect       r1, r2;
  HitBox *   hitbox1, *hitbox2;
  Transform *transform1, *transform2;

  qsort(_pair_buff, _pair_cnt, sizeof(CollisionPair), (__compar_fn_t)compr_pair);

  _pair_cnt = remove_duplicate_pairs(_pair_buff, _pair_cnt);

  for (u32 i = 0; i < _pair_cnt; ++i)
  {
    hitbox1 = ecs_get(ecs, _pair_buff[i].e1, HITBOX);
    hitbox2 = ecs_get(ecs, _pair_buff[i].e2, HITBOX);
    if ((BIT(hitbox1->category) & hitbox2->mask_bits) &&
        (BIT(hitbox2->category) & hitbox1->mask_bits))
    {
      transform1 = ecs_get(g_ecs, _pair_buff[i].e1, TRANSFORM);
      transform2 = ecs_get(g_ecs, _pair_buff[i].e2, TRANSFORM);
      get_bounding_rect(&r1, hitbox1, transform1);
      get_bounding_rect(&r2, hitbox2, transform2);
      if (rect_has_intersection(&r1, &r2))
      {
        ems_broadcast(MSG_COLLISION,
                           &(MSG_Collision){
                               _pair_buff[i].e1,
                               _pair_buff[i].e2,
                           });
      }
    }
  }
}

void collision_system_init()
{
  _rtree = rtree_new();
  ecs_connect(g_ecs, ECS_SIG_COMP_RMV, NULL, SLOT(on_component_remove));
}

void collision_system_fini()
{
  rtree_delete(_rtree);
  _rtree = NULL;
}

void collision_system_render_debug()
{
  rtree_draw_debug(_rtree, g_renderer, &g_viewport);
}

void collision_system_query(const AABB* aabb, Callback callback)
{
  rtree_query(_rtree, aabb, callback);
}

void collision_system_update()
{
  update_proxies(g_ecs);
  broad_phase(g_ecs);
  narrow_phase(g_ecs);
}

typedef struct
{
  // input
  const Rect* rect;
  u16         mask_bits;
  u16         max;

  // output

  u16*          cnt;
  ecs_entity_t* entities;
} CBQueryExVars;

static BOOL cb_query_ex(CBQueryExVars* vars, int proxy_id)
{
  ecs_entity_t entity;
  Transform*   transform;
  HitBox*      hitbox;
  Rect         bounding_rect;

  if (*vars->cnt == vars->max)
    return FALSE;

  entity = (ecs_entity_t)rtree_get_user_data(_rtree, proxy_id);

  transform = ecs_get(g_ecs, entity, TRANSFORM);
  hitbox    = ecs_get(g_ecs, entity, HITBOX);

  if (BIT(hitbox->category) & vars->mask_bits)
  {
    get_bounding_rect(&bounding_rect, hitbox, transform);
    if (rect_has_intersection(vars->rect, &bounding_rect))
    {
      vars->entities[(*vars->cnt)++] = entity;
    }
  }
  return TRUE;
}

void collision_system_query_ex(const RECT*   rect,
                               u16           mask_bits,
                               ecs_entity_t* entities,
                               u16*          cnt,
                               u16           max)
{
  AABB aabb;
  Rect _rect;

  aabb.lower_bound.x = rect->x;
  aabb.lower_bound.y = rect->y;
  aabb.upper_bound.x = rect->x + rect->w;
  aabb.upper_bound.y = rect->y + rect->h;

  rect_init(&_rect, rect->x, rect->y, rect->w, rect->h, 0.0);
  rtree_query(_rtree,
              &aabb,
              CALLBACK_1((&(CBQueryExVars){
                             .rect      = &_rect,
                             .mask_bits = mask_bits,
                             .max       = max,
                             .entities  = entities,
                             .cnt       = cnt,
                         }),
                         cb_query_ex));
}
