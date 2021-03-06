#include "global.h"
#include "entity_factory.h"
#include "json_helper.h"

ecs_entity_t (*const g_make_weapon_fn_tbl[NUM_WEAPONS])(Ecs*) = {
  make_spear, make_cleaver, make_golden_sword, make_anime_sword, make_staff, make_bow, make_katana,
};

ecs_entity_t (*const g_make_character_fn_tbl[NUM_JOBS])(Ecs*, Vec2) = {
  make_knight,
  make_lizzard,
  make_wizzard,
  make_elf,
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

void sprite_init(Sprite* s, u16 texture_id)
{
  s->texture_id = texture_id;
  SDL_QueryTexture(get_texture(texture_id), NULL, NULL, &s->rect.w, &s->rect.h);
}

ecs_entity_t create_weapon(Ecs* registry, u16 type)
{
  ASSERT(type < NUM_WEAPONS);
  return g_make_weapon_fn_tbl[type](registry);
}

Sprite animation_keyframe(const Animation* animation, u16 elapsed)
{
  Sprite current_key_frame;
  int    idx;
  idx = elapsed / animation->frame_duration;
  idx = idx % animation->count;

  current_key_frame.texture_id = animation->texture_id;
  current_key_frame.rect.x     = animation->offset_x + animation->sprite_width * idx;
  current_key_frame.rect.y     = animation->offset_y;
  current_key_frame.rect.w     = animation->sprite_width;
  current_key_frame.rect.h     = animation->sprite_height;
  return current_key_frame;
}
