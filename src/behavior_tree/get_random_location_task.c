#include "behavior_tree/nodes.h"
#include "components.h"
#include "resources.h"

BT_INST_DECL(GetRandomLocationTask, BTNode, {
  float radius;
  u32   bbResultKey;
})

BT_VTBL_DECL_EMPTY(GetRandomLocationTask, BTNode)

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
static BTStatus on_tick(GetRandomLocationTask* self, const BTUpdateContext* ctx)
{
  ecs_set(ctx->registry,
          ctx->entity,
          TEXT,
          &(Text){
              .value  = "get_random_location",
              .font   = get_font(FONT_ITEM_PICKED_UP),
              .effect = { .alignment = FC_ALIGN_CENTER,
                          .scale     = { 1.f, 1.f },
                          .color     = { 0xff, 0xff, 0xff, 0xff } },
          });
  Transform* transform;
  Vec2       random_location;
  if ((transform = ecs_get(ctx->registry, ctx->entity, TRANSFORM)))
  {
    random_location = get_random_location(transform->position, self->radius);
    blackboard_set_entry_as_vec2(ctx->blackboard, self->bbResultKey, random_location);
    return BT_STATUS_SUCCESS;
  }
  return BT_STATUS_FAILURE;
}

BT_VTBL_INITIALIZER(GetRandomLocationTask, BTNode, bt_node, {
  BT_NODE_VTBL(vtbl)->tick = (BTOnTickFunc)on_tick;
})

BTNode* bt_get_random_location_task_new(u32 resultKey, float radius)
{
  GetRandomLocationTask* self = bt_alloc(vtbl_inst());

  bt_node_init(BT_NODE(self));
  self->bbResultKey = resultKey;
  self->radius      = radius;
  return BT_NODE(self);
}
