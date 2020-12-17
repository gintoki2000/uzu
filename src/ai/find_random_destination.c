#include "find_random_destination.h"
#include <components.h>
#include <constances.h>

void      find_random_destination_vtbl_init(bt_NodeVtbl* vtbl);
bt_Status find_random_destination_exec(FindRandomDestination* self, Ecs* ecs, ecs_entity_t entity);
FindRandomDestination* find_random_destination_init(FindRandomDestination* self);

BT_NODE_VTBL_INST_FN(bt_Node, find_random_destination)
BT_ALLOC_FN(FindRandomDestination, find_random_destination)

void find_random_destination_vtbl_init(bt_NodeVtbl* vtbl)
{
  bt_node_vtbl_init((bt_NodeVtbl*)vtbl);
  vtbl->parent = bt_node_vtbl_inst();
  vtbl->exec   = (bt_exec_fn_t)find_random_destination_exec;
}

FindRandomDestination* find_random_destination_new()
{
  return find_random_destination_init(find_random_destination_alloc());
}

FindRandomDestination* find_random_destination_init(FindRandomDestination* self)
{
  bt_node_init((bt_Node*)self);
  return self;
}

static ecs_entity_t get_player(Ecs* ecs)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  ecs_data(ecs, PLAYER_TAG, &entities, NULL, &cnt);
  return cnt > 0 ? entities[0] : ECS_NULL_ENT;
}

static Vec2 get_entity_position(Ecs* ecs, ecs_entity_t entity)
{
  Transform* transform = ecs_get(ecs, entity, TRANSFORM);
  return transform->pos;
}

bt_Status find_random_destination_exec(FindRandomDestination* self, Ecs* ecs, ecs_entity_t entity)
{
  (void)self;

  // destination->x = rand() % (WIN_WIDTH - TILE_SIZE * 2) + TILE_SIZE * 2;
  // destination->y = rand() % (WIN_HEIGHT - TILE_SIZE * 2) + TILE_SIZE * 2;

  Vec2 player_pos = get_entity_position(ecs, get_player(ecs));
  Vec2 owner_pos  = get_entity_position(ecs, entity);

  Vec2 x = subv(player_pos, owner_pos);

  float l;
  if ((l = lengthv(x)) < 20)
  {
    return BT_STATUS_FAILURE;
  }

  btv_MoveDestination* destination = ecs_add(ecs, entity, BTV_MOVE_DESTINATION);
  vec2_scale_to_length(&x, l - 16);

  *destination = (Vec2){ owner_pos.x + x.x, owner_pos.y + x.y };

  return BT_STATUS_SUCCESS;
}
