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

Vec2 get_entity_position(Ecs* ecs, ecs_entity_t entity)
{
  return ((Transform*)ecs_get(ecs, entity, TRANSFORM))->pos;
}

BOOL set_entity_position(Ecs* ecs, ecs_entity_t entity, Vec2 pos)
{
  Transform* transform;
  if ((transform = ecs_get(ecs, entity, TRANSFORM)))
  {
    transform->pos = pos;
    return TRUE;
  }
  return FALSE;
}

BOOL set_entity_velocity(Ecs* ecs, ecs_entity_t entity, Vec2 vel)
{
  Motion* motion;
  if ((motion = ecs_get(ecs, entity, MOTION)))
  {
    motion->vel = vel;
    return TRUE;
  }
  return FALSE;
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

float get_distance_between_two_entities(Ecs* ecs, ecs_entity_t e1, ecs_entity_t e2)
{
  Vec2 p1, p2;
  p1 = get_entity_position(ecs, e1);
  p2 = get_entity_position(ecs, e2);
  return vec2_mag(vec2_sub(p1, p2));
}

void set_entity_hit_points(Ecs* ecs, ecs_entity_t entity, u16 hit_points)
{
  ((Health*)ecs_get(ecs, entity, HEALTH))->hit_points = hit_points;
}
