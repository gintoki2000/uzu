#include "ai/is_attacked.h"
#include "behaviour_tree.h"
#include "components.h"

struct BTCondition_IsAttacked
{
  BT_EXTEND_NODE(BTCondition)
};

static const BTNodeVtbl*       __vtbl_inst();
static void                    __vtbl_init(BTConditionVtbl* vtbl);
static BTCondition_IsAttacked* __init(BTCondition_IsAttacked* self);
static BOOL                    __pred(BTCondition_IsAttacked* self, Ecs* ecs, ecs_entity_t entity);

BT_STATIC_VTBL_INST_FN(BTCondition, _)
BT_ALLOC_FN(BTCondition_IsAttacked, _)

BTCondition_IsAttacked* bt_condition_is_attacked_new()
{
  return __init(__alloc());
}

static void __vtbl_init(BTConditionVtbl* vtbl)
{
  bt_condition_vtbl_init(vtbl);
  vtbl->pred = (bt_pred_fn_t)__pred;
}

static BTCondition_IsAttacked* __init(BTCondition_IsAttacked* self)
{
  bt_condition_init((BTCondition*)self, TRUE);
  return self;
}

static BOOL __pred(SDL_UNUSED BTCondition_IsAttacked* self, Ecs* ecs, ecs_entity_t entity)
{
  return ecs_has(ecs, entity, ATTACKER);
}
