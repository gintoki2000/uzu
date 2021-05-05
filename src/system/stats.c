#include "components.h"
#include "config.h"
extern Ecs* g_ecs;
static void on_stats_added(SDL_UNUSED void* arg, const EcsComponentAdded* event)
{
  ecs_add(g_ecs, event->entity, HEALTH);
  ecs_add(g_ecs, event->entity, MOVE_SPEED);
  ecs_add(g_ecs, event->entity, MANA);
  ecs_add(g_ecs, event->entity, DEFENSE);

  ecs_add(g_ecs, event->entity, VITALITY_CHANGED);
  ecs_add(g_ecs, event->entity, AGILITY_CHANGED);
  ecs_add(g_ecs, event->entity, INTELLIGENT_CHANGED);
  ecs_add(g_ecs, event->entity, STRENGTH_CHANGED);
}

static int total_points(const Stat* s)
{
  double total_modifier = (s->modifier[0] + s->modifier[1] + s->modifier[2]) / 100.0;
  int    total_stat     = s->base * (1.0 + total_modifier);
  return total_stat;
}

static void vitality(void)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  Health*       health;
  const Stats*  stats;
  ecs_raw(g_ecs, VITALITY_CHANGED, &entities, NULL, &cnt);
  for (int i = cnt - 1; i >= 0; --i)
  {
    health          = ecs_get(g_ecs, entities[i], HEALTH);
    stats           = ecs_get(g_ecs, entities[i], STATS);
    health->max     = total_points(&stats->vitality);
    health->current = health->max;
    ecs_rmv(g_ecs, entities[i], VITALITY_CHANGED);
  }
}

static void agility(void)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  Stats*        stats;
  MoveSpeed*    move_speed;
  int           points;

  ecs_raw(g_ecs, AGILITY_CHANGED, &entities, NULL, &cnt);
  for (int i = cnt - 1; i >= 0; --i)
  {
    stats             = ecs_get(g_ecs, entities[i], STATS);
    move_speed        = ecs_get(g_ecs, entities[i], MOVE_SPEED);
    points            = total_points(&stats->agility);
    move_speed->value = points * 10;
    ecs_rmv(g_ecs, entities[i], AGILITY_CHANGED);
  }
}

static void intelligent(void)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  Mana*         mana;
  Stats*        stats;
  int           new_cap;

  ecs_raw(g_ecs, INTELLIGENT_CHANGED, &entities, NULL, &cnt);
  for (int i = cnt - 1; i >= 0; --i)
  {
    mana          = ecs_get(g_ecs, entities[i], MANA);
    stats         = ecs_get(g_ecs, entities[i], STATS);
    new_cap       = total_points(&stats->intelligent);
    mana->current = new_cap - mana->current;
    mana->max     = new_cap;
    ecs_rmv(g_ecs, entities[i], INTELLIGENT_CHANGED);
  }
}

void stats_system_init(void)
{
  signal_connect(ecs_on_construct(g_ecs, STATS), CALLBACK_2(on_stats_added));
}

void stats_system(void)
{
  vitality();
  intelligent();
  agility();
}
