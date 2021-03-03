#include "btcondition.h"
#include "components.h"

struct BTCondition_IsAttacked
{
  BT_EXTEND_NODE(BTCondition)
};

#define NODE BTCondition_IsAttacked

static const BTNodeVtbl* __vtbl_inst(void);
static void              __vtbl_init(BTConditionVtbl* vtbl);
static NODE*             __init(NODE* self, BOOL invert_result);
static BOOL              __pred(NODE* self, Ecs* ecs, ecs_entity_t entity);

BT_STATIC_VTBL_INST_FN(BTCondition, _)
BT_ALLOC_FN(BTCondition_IsAttacked, _)

NODE* bt_condition_is_attacked_new(BOOL invert_result)
{
  return __init(__alloc(), invert_result);
}

static void __vtbl_init(BTConditionVtbl* vtbl)
{
  bt_condition_vtbl_init(vtbl);
  vtbl->pred = (bt_pred_fn_t)__pred;
}

static NODE* __init(NODE* self, BOOL invert_result)
{
  bt_condition_init((BTCondition*)self, TRUE, invert_result);
  return (self);
}

static BOOL __pred(SDL_UNUSED NODE* self, Ecs* ecs, ecs_entity_t entity)
{
  return ecs_has (ecs, entity, ATTACKER);
}
