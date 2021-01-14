#include "types.h"
#include "json_helper.h"
#include "entity_factory.h"

ecs_entity_t (*const g_weapon_create_fn_tbl[NUM_WEAPONS])(Ecs*, u16) = {
  NULL,
  make_cleaver,
  make_golden_sword,
  make_anime_sword,
  NULL,
  NULL,
};
ecs_entity_t (*const g_char_create_fn_tbl[NUM_CLASSES])(Ecs*, Vec2)  = {
  make_knight,
  NULL,
  make_wizzard,
  NULL,
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
