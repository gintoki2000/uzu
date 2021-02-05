#include "global.h"
#include "entity_factory.h"
#include "json_helper.h"

ecs_entity_t (*const g_weapon_create_fn_tbl[NUM_WEAPONS])(Ecs*, u16) = {
  make_spear, make_cleaver, make_golden_sword, make_anime_sword, make_staff, NULL,
};

ecs_entity_t (*const g_char_create_fn_tbl[NUM_JOBS])(Ecs*, Vec2) = {
  make_knight,
  make_dragon,
  make_wizzard,
  NULL,
};

const u16 g_pickupable_to_item_type_id_tbl[] = {
  [PICKUPABLE_RED_FLASK]     = ITEM_TYPE_RED_FLASK,
  [PICKUPABLE_BIG_RED_FLASK] = ITEM_TYPE_BIG_RED_FLASK,
  [PICKUPABLE_BLUE_FLASK]    = ITEM_TYPE_BLUE_FLASK,
  [PICKUPABLE_COIN]          = ITEM_TYPE_ID_NULL,
  [PICKUPABLE_KEY_1_1]       = ITEM_TYPE_KEY_1_1,
};

Conversation* conversation_init(Conversation* self)
{
  self->name      = NULL;
  self->responses = ptr_array_new(free);
  self->sentences = ptr_array_new(free);
  return self;
}

void conversation_fini(Conversation* self)
{
  if (self->name != NULL)
  {
    free(self->name);
    self->name = NULL;
  }
  ptr_array_delete(self->responses);
  ptr_array_delete(self->sentences);
  self->responses = NULL;
  self->sentences = NULL;
}
