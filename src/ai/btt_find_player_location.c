#include "btt_find_player_location.h"
#include "path_finder.h"
#include <components.h>
#include <constances.h>

BT_DEF_NODE_VTBL(BTT_FIND_PLAYER_LOCATION_VTBL,
                 bt_node_fini,
                 bt_node_start_impl,
                 btt_find_player_location_exec);

btt_FindPlayerLocation* btt_find_player_location_new()
{
  btt_FindPlayerLocation* btt = malloc(sizeof(btt_FindPlayerLocation));
  BT_NODE(btt)->vtbl          = &BTT_FIND_PLAYER_LOCATION_VTBL;
  return btt_find_player_location_init(btt);
}

btt_FindPlayerLocation* btt_find_player_location_init(btt_FindPlayerLocation* self)
{
  bt_node_init((bt_Node*)self);
  return self;
}

void btt_find_player_location_exec(btt_FindPlayerLocation* self, Ecs* ecs, ecs_entity_t entity)
{
  INFO("executing..\n");
  btv_Path*     path;
  ecs_entity_t* player;

  Transform* owner_tx;
  Transform* player_tx;

  pf_Node nodes[100];
  int     cnt;

  int player_x, player_y, owner_x, owner_y;

  ecs_data(ecs, PLAYER_TAG, &player, NULL, NULL);

  owner_tx  = ecs_get(ecs, entity, TRANSFORM);
  player_tx = ecs_get(ecs, player[0], TRANSFORM);

  owner_x = owner_tx->pos.x / TILE_SIZE;
  owner_y = owner_tx->pos.y / TILE_SIZE;

  player_x = player_tx->pos.x / TILE_SIZE;
  player_y = player_tx->pos.y / TILE_SIZE;

  if (find_path(owner_x, owner_y, player_x, player_y, nodes, &cnt))
  {
    INFO("finish!\n");
    path = ecs_add(ecs, entity, BTV_PATH);
    memcpy(path->nodes, nodes, cnt * sizeof(pf_Node));
    path->cnt  = cnt;
    path->curr = 0;
    bt_finish_exec(self, BT_STATUS_SUCCESS);
  }
  else
  {
    INFO("fail\n");
    bt_finish_exec(self, BT_STATUS_FAILURE);
  }
}
