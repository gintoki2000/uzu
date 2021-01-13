#include "utils.h"
#include <components.h>
ecs_entity_t get_player(Ecs* ecs)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  ecs_raw(ecs, PLAYER_TAG, &entities, NULL, &cnt);
  return cnt > 0 ? entities[0] : ECS_NULL_ENT;
}

Vec2 get_player_position(Ecs* ecs)
{
  return get_entity_position(ecs, get_player(ecs));
}

void move_player_to(Ecs* ecs, Vec2 dest)
{
  ecs_entity_t player = get_player(ecs);
  ASSERT(player != ECS_NULL_ENT);
  Transform* transform = ecs_get(ecs, player, TRANSFORM);
  transform->pos       = dest;
}

Vec2 get_entity_position(Ecs* ecs, ecs_entity_t entity)
{
  return ((Transform*)ecs_get(ecs, entity, TRANSFORM))->pos;
}

ecs_entity_t find_ladder(Ecs* ecs, const char* _name)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  LevelSwitcher* switchers;
  Name*          name;

  ecs_raw(ecs, LEVEL_SWITCHER, &entities, (pointer_t*)&switchers, &cnt);
  for (int i = 0; i < cnt; ++i)
  {
    if ((name = ecs_get(ecs, entities[i], NAME)))
    {
      if (strcmp(_name, name->value) == 0)
      {
        return entities[i];
      }
    }
  }
  return ECS_NULL_ENT;
}

BOOL equip(Ecs* ecs, ecs_entity_t entity, ecs_entity_t weapon)
{
  Equipment*  equipment;
  WeaponBase* weapon_core;

  if (entity == ECS_NULL_ENT || weapon == ECS_NULL_ENT)
    return FALSE;

  equipment   = ecs_get(ecs, entity, EQUIPMENT);
  weapon_core = ecs_get(ecs, weapon, WEAPON_BASE);

  if (weapon_core == NULL)
    return FALSE;
  equipment->weapon   = weapon;
  weapon_core->wearer = entity;

  return TRUE;
}

ecs_entity_t find_entity(Ecs* ecs, const char* name)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  Name*         names;

  ecs_raw(ecs, NAME, &entities, (pointer_t*)&names, &cnt);
  for (int i = 0; i < cnt; ++i)
  {
    if (strcmp(names[i].value, name) == 0)
    {
      return entities[i];
    }
  }
  return ECS_NULL_ENT;
}
