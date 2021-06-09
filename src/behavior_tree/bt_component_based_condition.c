#include "behavior_tree/bt_decorator_node.h"
#include "behavior_tree/bt_node.h"
#include "behavior_tree/common_enums.h"

BT_INST_DECL(bt_ComponentBasedCondition, bt_DecoratorNode, {
  ecs_size_t        componentType;
  bt_ComponentQuery componentQuery;
})

BT_VTBL_DECL_EMPTY(bt_ComponentBasedCondition, bt_DecoratorNode)

#define super BT_DECORATOR(self)

static bt_ComponentBasedCondition*
init(bt_ComponentBasedCondition* self, ecs_size_t componentType, bt_ComponentQuery componentQuery)
{
  bt_decorator_node_init(super);
  self->componentQuery = componentQuery;
  self->componentType  = componentType;
  return self;
}

static bt_Status on_tick(bt_ComponentBasedCondition* self, const bt_UpdateContext* ctx)
{
  BOOL has_given_component = ecs_has(ctx->registry, ctx->entity, self->componentType);
  return (self->componentQuery == COMP_QUERY_HAS ? has_given_component : !has_given_component)
             ? bt_decorator_node_on_tick(super, ctx)
             : BT_STATUS_FAILURE;
}

BT_VTBL_INITIALIZER(bt_ComponentBasedCondition, bt_DecoratorNode, bt_decorator_node, {
  BT_NODE_VTBL(vtbl)->tick = (bt_OnTickFunc)on_tick;
})

bt_DecoratorNode* bt_component_base_condition_new(ecs_entity_t      componentType,
                                                  bt_ComponentQuery componentQuery)
{
  bt_ComponentBasedCondition* node = SDL_malloc(sizeof(bt_ComponentBasedCondition));
  BT_NODE(node)->vtbl              = vtbl_inst();
  return BT_DECORATOR(init(node, componentType, componentQuery));
}
