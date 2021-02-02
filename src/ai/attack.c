#include "ai/attack.h"

#include "../../include/entity_utils.h"
#include "components.h"
#include "constances.h"

struct BTTask_Attack
{
  BT_EXTEND_NODE(BTNode)
  BOOL is_running;
  u16  mode;
};

static void __vtbl_init(BTNodeVtbl* vtbl);

static BTTask_Attack* __init(BTTask_Attack* self, u16 mode);

static void __finish(BTTask_Attack* self, Ecs* ecs, ecs_entity_t entity, BTStatus finish_status);

static void __abort(BTTask_Attack* self, Ecs* ecs, ecs_entity_t entity);

static BTStatus __exec(BTTask_Attack* self, Ecs* ecs, ecs_entity_t entity);

BT_STATIC_VTBL_INST_FN(BTNode, _)
BT_ALLOC_FN(BTTask_Attack, _)

static void __vtbl_init(BTNodeVtbl* vtbl)
{
  bt_node_vtbl_init(vtbl);
  vtbl->parent = bt_node_vtbl_inst();
  vtbl->exec   = (bt_exec_fn_t)__exec;
  vtbl->abort  = (bt_abort_fn_t)__abort;
  vtbl->finish = (bt_finish_fn_t)__finish;
}

BTTask_Attack* bt_task_attack_new(u16 mode)
{
  return __init(__alloc(), mode);
}

static BTTask_Attack* __init(BTTask_Attack* self, u16 mode)
{
  bt_node_init((BTNode*)self);
  self->is_running = FALSE;
  self->mode       = mode;
  return self;
}

static void __abort(BTTask_Attack* self, SDL_UNUSED Ecs* ecs, SDL_UNUSED ecs_entity_t entity)
{
  self->is_running = FALSE;
}

static void __finish(SDL_UNUSED BTTask_Attack* self,
                     SDL_UNUSED Ecs*         ecs,
                     SDL_UNUSED ecs_entity_t entity,
                     SDL_UNUSED BTStatus     finish_status)
{
  self->is_running = FALSE;
}

static BTStatus __exec(BTTask_Attack* self, Ecs* ecs, ecs_entity_t entity)
{
  Controller*  controller;
  ecs_entity_t target;
  Vec2         entity_position, target_position;
  Transform*   transform;
  Equipment*   equipment;

  controller = ecs_get(ecs, entity, CONTROLLER);

  switch (self->mode)
  {
  case BTTASK_ATTACK_MODE_PLAYER:
    target = get_player(ecs);
    break;
  case BTTASK_ATTACK_MODE_ATTACKER:
  {
    Attacker* attacker = ecs_get(ecs, entity, ATTACKER);
    target = attacker && ecs_is_valid(ecs, attacker->value) ? attacker->value : ECS_NULL_ENT;
  }
  break;
  }

  if (!self->is_running)
  {
    equipment = ecs_get(ecs, entity, EQUIPMENT);
    target    = get_player(ecs);
    if (controller->in_action || ecs_has(ecs, entity, INPUT_BLOCKER) || target == ECS_NULL_ENT ||
        equipment->weapon == ECS_NULL_ENT)
      return BT_STATUS_FAILURE;
    transform       = ecs_get(ecs, entity, TRANSFORM);
    entity_position = transform->position;
    target_position = get_entity_position(ecs, target);

    transform->hdir = (entity_position.x < target_position.x) ? 1 : -1;

    controller->action = CHARACTER_ACTION_REGULAR_ATK;
    self->is_running   = TRUE;
    return BT_STATUS_RUNNING;
  }
  else
  {
    if (controller->in_action)
      return BT_STATUS_RUNNING;
    else
    {
      self->is_running = FALSE;
      return BT_STATUS_SUCCESS;
    }
  }
}
