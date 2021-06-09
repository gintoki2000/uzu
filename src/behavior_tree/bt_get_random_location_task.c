#include "behavior_tree/bt_get_random_location_task.h"
#include "behavior_tree/bt_node.h"
#include "components.h"
#include "resources.h"

BT_NODE_DECL(bt_GetRandomLocationTask, bt_Node, {
  float radius;
  u32   key_result;
})

INLINE float randf()
{
  return (float)rand() / (float)RAND_MAX;
}

static Vec2 get_random_location(Vec2 origin, float radius)
{
  Vec2 p;

  float a = (rand() % 360) * DEG_TO_RAD;
  float l = randf() * radius;

  p.x = SDL_cosf(a) * l;
  p.y = SDL_sinf(a) * l;
  return vec2_add(origin, p);
}

static bt_Status on_tick(bt_GetRandomLocationTask* self, const bt_UpdateContext* ctx)
{
  Transform* transform;
  Vec2       randomLocation;
  if ((transform = ecs_get(ctx->registry, ctx->entity, TRANSFORM)))
  {
    randomLocation = get_random_location(transform->position, self->radius);
    blackboard_set_entry_as_vec2(ctx->blackboard, self->key_result, randomLocation);
    return BT_STATUS_SUCCESS;
  }
  return BT_STATUS_FAILURE;
}

bt_Node* bt_get_random_location_task_create(u32 key_result, float radius)
{
  /*allocate memory*/
  bt_GetRandomLocationTask* self = SDL_malloc(sizeof(bt_GetRandomLocationTask));

  /*init base */
  bt_node_init(BT_NODE(self), BT_NODE_TYPE_TASK);

  /*overide virtual methods*/
  BT_NODE(self)->tick = (bt_OnTickFunc)on_tick;

  /*init*/
  self->radius     = radius;
  self->key_result = key_result;
  return (bt_Node*)self;
}
