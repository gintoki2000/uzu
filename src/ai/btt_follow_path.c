#include "btt_follow_path.h"
#include <components.h>
#include <constances.h>

BT_DEF_NODE_VTBL(btt_follow_path_vtbl, bt_node_fini, btt_follow_path_start, btt_follow_path_exec);

btt_FollowPath* btt_follow_path_new()
{
  btt_FollowPath* node   = malloc(sizeof(btt_FollowPath));
  ((bt_Node*)node)->vtbl = &btt_follow_path_vtbl;
  return btt_follow_path_init(node);
}

btt_FollowPath* btt_follow_path_init(btt_FollowPath* self)
{
  bt_node_init((bt_Node*)self);
  return self;
}

void btt_follow_path_start(btt_FollowPath* self)
{
  bt_node_start_impl((bt_Node*)self);
  self->curr = 0;
}

void btt_follow_path_exec(btt_FollowPath* self, Ecs* ecs, ecs_entity_t entity)
{
  btv_Path*  path;
  Transform* transform;
  Motion*    motion;
  Vec2       target;
  Vec2       desired;
  Vec2       steer;
  float      d;
  Vec2       pos;

  if ((path = ecs_get(ecs, entity, BTV_PATH)) && (transform = ecs_get(ecs, entity, TRANSFORM)) &&
      (motion = ecs_get(ecs, entity, MOTION)))
  {
    target.x = (path->nodes[path->curr].x + 0.5) * TILE_SIZE;
    target.y = (path->nodes[path->curr].y + 0.5) * TILE_SIZE;
    pos.x    = transform->pos.x;
    pos.y    = transform->pos.y;

    desired = subv(target, pos);
    d       = normalizev(&desired);
    if (d < 2)
    {
      path->curr++;
      if (path->curr >= path->cnt)
      {
        motion->vel = VEC2_ZERO;
        ecs_rmv(ecs, entity, BTV_PATH);
        INFO("Succesed!\n");
        bt_finish_exec(self, BT_STATUS_SUCCESS);
      }
    }
    else
    {
      desired = mulv(desired, motion->max_speed);
      // steer   = subv(desired, motion->vel);
      // steer   = truncatev(steer, 10);
      // motion->acc.x += steer.x;
      // motion->acc.y += steer.y;
      motion->vel = desired;
      // INFO("processing node #%d...\n", path->curr);
      bt_finish_exec(self, BT_STATUS_RUNNING);
    }
  }
  else
  {
    INFO("e" ECS_ENT_FMT_PATTERN " there is no path or entity is not moveable object\n",
         ECS_ENT_FMT_VARS(entity));
    bt_finish_exec(self, BT_STATUS_FAILURE);
  }
}
