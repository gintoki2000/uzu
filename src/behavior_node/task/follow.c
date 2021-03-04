#include "bttask.h"
#include "components.h"
struct BTTask_Follow
{
  BT_EXTEND_NODE(BTNode)
  float distance;
};

struct BTTask_FollowVtbl
{
  BT_EXTEND_VTBL(BTNodeVtbl)
};

static const BTNodeVtbl* __vtbl_inst();
static void              __vtbl_init(BTNodeVtbl* vtbl);

#define NODE BTTask_Follow

static NODE*    __init(NODE*, float);
static void     __abort(NODE*, Ecs*, ecs_entity_t);
static void     __finish(NODE*, Ecs*, ecs_entity_t, BTStatus);
static BTStatus __exec(NODE*, Ecs*, ecs_entity_t);

BT_STATIC_VTBL_INST_FN(BTNode, _)
BT_ALLOC_FN(NODE, _)

void __vtbl_init(BTNodeVtbl* vtbl)
{
  bt_node_vtbl_init(vtbl);
  vtbl->parent = bt_node_vtbl_inst();
  vtbl->abort  = (bt_abort_fn_t)__abort;
  vtbl->exec   = (bt_exec_fn_t)__exec;
  vtbl->finish = (bt_finish_fn_t)__finish;
}

NODE* bt_task_follow_new(float distance)
{
  return __init(__alloc(), distance);
}

NODE* __init(NODE* self, float distance)
{
  bt_node_init((BTNode*)self);
  self->distance = distance;
  return self;
}

void __finish(SDL_UNUSED NODE* self,
              SDL_UNUSED Ecs*         ecs,
              SDL_UNUSED ecs_entity_t entity,
              SDL_UNUSED BTStatus     finish_status)
{
}

void __abort(NODE* self, SDL_UNUSED Ecs* ecs, SDL_UNUSED ecs_entity_t entity)
{
}

BTStatus __exec(NODE* self, SDL_UNUSED Ecs* ecs, SDL_UNUSED ecs_entity_t entity)
{
}

#undef NODE
