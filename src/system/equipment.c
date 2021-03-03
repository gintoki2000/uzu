#include "components.h"
#include "system/game_logic.h"

extern Ecs* g_ecs;

void equipment_system()
{
  ecs_entity_t*    entities;
  ecs_size_t       count;
  Hand*            hand;
  FacingDirection* facing_diection;
  float            horizontal_axis;
  Visual*          weapon_visual;
  Transform*       weapon_transfrom;
  Transform*       self_transform;

  ecs_raw(g_ecs, HAND, &entities, (void**)&hand, &count);
  for (int i = 0; i < count; ++i)
  {
    if (hand[i].weapon != ECS_NULL_ENT)
    {
      facing_diection  = ecs_get(g_ecs, entities[i], FACING_DIRECTION);
      weapon_visual    = ecs_get(g_ecs, hand[i].weapon, VISUAL);
      self_transform   = ecs_get(g_ecs, entities[i], TRANSFORM);
      weapon_transfrom = ecs_get(g_ecs, hand[i].weapon, TRANSFORM);

      horizontal_axis     = signf(facing_diection->value.x);
      weapon_visual->flip = horizontal_axis > 0.f ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;

      weapon_transfrom->position.x =
          self_transform->position.x +
          horizontal_axis * (hand[i].attach_point.x + hand[i].adjustment.x);
      weapon_transfrom->position.y =
          self_transform->position.y + hand[i].attach_point.y + hand[i].adjustment.y;
    }
  }
}
