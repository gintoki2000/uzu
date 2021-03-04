#include "components.h"
#include "system/game_logic.h"

extern Ecs* g_ecs;

void equipment_system()
{
  ecs_entity_t*     entities;
  ecs_size_t        count;
  Hand*             hand;
  FacingDirection*  facing_diection;
  float             horizontal_axis;
  Visual*           weapon_visual;
  Transform*        weapon_transfrom;
  Transform*        self_transform;
  Vec2              weapon_position;
  WeaponAttributes* weapon_attrs;
  float             x, y, s, c, theta;

  ecs_raw(g_ecs, HAND, &entities, (void**)&hand, &count);
  for (int i = 0; i < count; ++i)
  {
    if (hand[i].weapon != ECS_NULL_ENT)
    {
      facing_diection  = ecs_get(g_ecs, entities[i], FACING_DIRECTION);
      self_transform   = ecs_get(g_ecs, entities[i], TRANSFORM);
      weapon_visual    = ecs_get(g_ecs, hand[i].weapon, VISUAL);
      weapon_transfrom = ecs_get(g_ecs, hand[i].weapon, TRANSFORM);
      weapon_attrs     = ecs_get(g_ecs, hand[i].weapon, WEAPON_ATTRIBUTES);

      horizontal_axis = signf(facing_diection->value.x);

      if (weapon_attrs->rotate_hand)
      {
        theta             = SDL_atan2f(facing_diection->value.y, facing_diection->value.x);
        x                 = hand[i].attach_point.x + hand[i].adjustment.x;
        y                 = hand[i].attach_point.y + hand[i].adjustment.y;
        s                 = SDL_sinf(theta);
        c                 = SDL_cosf(theta);
        weapon_position.x = x * c - y * s;
        weapon_position.y = x * s + y * c;
        weapon_position.x += hand[i].original_point.x;
        weapon_position.y += hand[i].original_point.y;
        weapon_position.x += self_transform->position.x;
        weapon_position.y += self_transform->position.y - self_transform->z;

        weapon_transfrom->position = weapon_position;
        weapon_transfrom->rotation = theta * 57.2957795f;
      }
      else
      {
        weapon_position.x = self_transform->position.x;
        weapon_position.y = self_transform->position.y - self_transform->z;

        weapon_position.x += hand[i].original_point.x;
        weapon_position.y += hand[i].original_point.y;

        weapon_position.x += horizontal_axis * (hand[i].attach_point.x + hand[i].adjustment.x);
        weapon_position.y += hand[i].attach_point.y + hand[i].adjustment.y;

        weapon_transfrom->position = weapon_position;
        weapon_visual->flip        = horizontal_axis > 0.f ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
      }
    }
  }
}
