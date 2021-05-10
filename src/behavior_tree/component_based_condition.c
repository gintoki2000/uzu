#include "behavior_tree/base.h"
#include "behavior_tree/common_enums.h"

typedef struct
{
  BTDecorator    _base;
  ecs_size_t     componentType;
  ComponentQuery componentQuery;
} ComponentBasedCondition;

typedef struct
{
  BTDecorator _base;
} ComponentBasedConditionVtbl;


#define super ((BTDecorator*)self)

static ComponentBasedCondition*
initialize(ComponentBasedCondition* self, ecs_size_t componentType, ComponentQuery componentQuery)
{
  bt_decorator_init(super);
  self->componentQuery = componentQuery;
  self->componentType  = componentType;
  return self;
}

static BTStatus on_tick(ComponentBasedCondition* self, const BTUpdateContext* ctx)
{
  BOOL has_given_component = ecs_has(ctx->registry, ctx->entity, self->componentType);
  return (self->componentQuery == COMP_QUERY_HAS ? has_given_component : !has_given_component)
             ? bt_decorator_on_tick(super, ctx)
             : BT_STATUS_FAILURE;
}

BT_VTBL_INITIALIZER(ComponentBasedCondition, BTDecorator, bt_decorator, {
  ((BTNodeVtbl*)vtbl)->tick = (BTOnTickFunc)on_tick;
})

BT_INST_ALLOC_FN(ComponentBasedCondition, component_base_condition)

BTDecorator* bt_component_base_condition_new(ecs_entity_t   componentType,
                                             ComponentQuery componentQuery)
{
  return BT_DECORATOR(initialize(component_base_condition_alloc(), componentType, componentQuery));
}
