#include "behavior_tree/base.h"
#include "components.h"
#include "resources.h"

BT_VTBL_DECL_EMPTY(WaitTask, BTNode)
BT_INST_DECL(WaitTask, BTNode, {
  u32 duration;
  u32 remaining;
})

static WaitTask* init(WaitTask* self, u32 duration)
{
  bt_node_init((BTNode*)self);
  self->duration  = duration;
  self->remaining = 0;
  return self;
}

static void on_start(WaitTask* self, SDL_UNUSED const BTUpdateContext* ctx)
{
  self->remaining = self->duration;
}

static BTStatus on_tick(WaitTask* self, SDL_UNUSED const BTUpdateContext* ctx)
{
  ecs_set(ctx->registry,
          ctx->entity,
          TEXT,
          &(Text){
              .value  = "wait",
              .font   = get_font(FONT_ITEM_PICKED_UP),
              .effect = { .alignment = FC_ALIGN_CENTER,
                          .scale     = { 1.f, 1.f },
                          .color     = { 0xff, 0xff, 0xff, 0xff } },
          });
  --self->remaining;
  return self->remaining > 0 ? BT_STATUS_RUNNING : BT_STATUS_SUCCESS;
}

BT_VTBL_INITIALIZER(WaitTask, BTNode, bt_node, {
  BT_NODE_VTBL(vtbl)->start = (BTOnStartFunc)on_start;
  BT_NODE_VTBL(vtbl)->tick  = (BTOnTickFunc)on_tick;
})

#define ALLOC_MEM() bt_alloc(vtbl_inst())

BTNode* bt_wait_task_new(u32 duration)
{
  return BT_NODE(init(ALLOC_MEM(), duration));
}
