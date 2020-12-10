#include "btt_move_to.h"
#include <components.h>
#include <ecs/ecs.h>

#define MAX_SPEED 99
#define MAX_FORCE 10

static const bt_NodeVtbl BTT_MOVE_TO_VTBL = {
  .fini  = (destroy_fn_t)bt_node_fini,
  .start = (bt_start_fn_t)bt_node_start_impl,
  .exec  = (bt_exec_fn_t)btt_move_to_exec,
};

static btt_MoveTo* btt_move_to_init(btt_MoveTo* self);

btt_MoveTo* btt_move_to_new()
{
  btt_MoveTo* move_to    = malloc(sizeof(btt_MoveTo));
  BT_NODE(move_to)->vtbl = &BTT_MOVE_TO_VTBL;
  return btt_move_to_init(move_to);
}
static btt_MoveTo* btt_move_to_init(btt_MoveTo* self)
{
  bt_node_init((bt_Node*)self);
  return self;
}

void btt_move_to_exec(btt_MoveTo* self, Ecs* ecs, ecs_entity_t entity)
{
  // INFO("executing...\n");
  Motion*              motion;
  Transform*           transform;
  btv_MoveDestination* destination;

  destination = ecs_get(ecs, entity, BTV_MOVE_DESTINATION);
  if (destination == NULL)
  {
    INFO("there is no destination\n");
    bt_finish_exec(self, BT_STATUS_FAILURE);
  }

  if ((motion = ecs_get(ecs, entity, MOTION)) && (transform = ecs_get(ecs, entity, TRANSFORM)))
  { 

    Vec2 steer;
    Vec2 desired = subv(*destination, transform->pos);
    int  d       = lengthv(desired);
    normalizev(&desired);
    if (d < 50)
    {
      motion->vel = VEC2_ZERO;
      motion->acc = VEC2_ZERO;
      ecs_rmv(ecs, entity, BTV_MOVE_DESTINATION);
      bt_finish_exec(self, BT_STATUS_SUCCESS);
    }
    else
    {
      desired = mulv(desired, MAX_SPEED);
      steer   = subv(desired, motion->vel);
      steer   = truncatev(steer, MAX_FORCE);
      motion->acc.x += steer.x;
      motion->acc.y += steer.y;
      bt_finish_exec(self, BT_STATUS_RUNNING);
    }
  }
  else
  {
    INFO("e" ECS_ENT_FMT_PATTERN " is not moveable object\n", ECS_ENT_FMT_VARS(entity));
    bt_finish_exec(self, BT_STATUS_FAILURE);
  }
}
