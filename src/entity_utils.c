#include "entity_utils.h"
#include "components.h"
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
  return ((Transform*)ecs_get(ecs, entity, TRANSFORM))->position;
}

BOOL set_entity_position(Ecs* ecs, ecs_entity_t entity, Vec2 pos)
{
  Transform* transform;
  if ((transform = ecs_get(ecs, entity, TRANSFORM)))
  {
    transform->position = pos;
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

  LadderAttributes* switchers;
  Name*             name;

  ecs_raw(ecs, LADDER_ATTRIBUTES, &entities, (pointer_t*)&switchers, &cnt);
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
  Hand* equipment;

  if (entity == ECS_NULL_ENT || weapon == ECS_NULL_ENT)
    return FALSE;

  equipment = ecs_get(ecs, entity, HAND);

  ecs_set(ecs, weapon, HOLDER, &(Holder){ .value = entity });
  if (equipment->weapon != ECS_NULL_ENT)
    ecs_add(ecs, equipment->weapon, DESTROYED_TAG);
  equipment->weapon = weapon;

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

u16 get_entity_hit_points(Ecs* ecs, ecs_entity_t entity)
{
  return ((Health*)ecs_get(ecs, entity, HEALTH))->hit_points;
}

u16 get_entity_conversation(Ecs* ecs, ecs_entity_t entity)
{
  return ((Dialogue*)ecs_get(ecs, entity, DIALOGUE))->conversation_id;
}

const char* get_entity_name(Ecs* ecs, ecs_entity_t entity)
{
  return ((Name*)ecs_get(ecs, entity, NAME))->value;
}

Vec2 get_entity_velocity(Ecs* ecs, ecs_entity_t entity)
{
  return ((Motion*)ecs_get(ecs, entity, MOTION))->vel;
}

float get_entity_vz(Ecs* ecs, ecs_entity_t entity)
{
  return ((Motion*)ecs_get(ecs, entity, MOTION))->vz;
}
u16 get_entity_mana_points(Ecs* ecs, ecs_entity_t entity)
{
  return ((ManaPool*)ecs_get(ecs, entity, MANA_POOL))->mana_points;
}

void set_entity_mana_points(Ecs* ecs, ecs_entity_t entity, u16 mana_points)
{
  ((ManaPool*)ecs_get(ecs, entity, MANA_POOL))->mana_points = mana_points;
}
void set_spell(Ecs* ecs, ecs_entity_t entity, u16 spell_id)
{
  ((AttunementSlot*)ecs_get(ecs, entity, ATTUNEMENT_SLOT))->spell_id = spell_id;
}
u16 get_spell(Ecs* ecs, ecs_entity_t entity)
{
  return ((AttunementSlot*)ecs_get(ecs, entity, ATTUNEMENT_SLOT))->spell_id;
}
u16 get_weapon_type_id(Ecs* ecs, ecs_entity_t weapon)
{
  return ((const WeaponAttributes*)ecs_get(ecs, weapon, WEAPON_ATTRIBUTES))->type_id;
}
ecs_entity_t get_equiped_weapon(Ecs* ecs, ecs_entity_t holder)
{
  return ((const Hand*)ecs_get(ecs, holder, HAND))->weapon;
}
u16 get_equiped_weapon_type_id(Ecs* ecs, ecs_entity_t holder)
{
  return get_weapon_type_id(ecs, get_equiped_weapon(ecs, holder));
}
