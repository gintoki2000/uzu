#include "components.h"
#include "config.h"
extern ecs_Registry* gRegistry;

#define STAT_MAX 16
static int _manaTable[STAT_MAX + 1];
static int _healthTable[STAT_MAX + 1];

static void on_stats_added(SDL_UNUSED void* arg, const ecs_ComponentAdded* event)
{
  ecs_add(gRegistry, event->entity, HEALTH);
  ecs_add(gRegistry, event->entity, MOVE_SPEED);
  ecs_add(gRegistry, event->entity, MANA);
  ecs_add(gRegistry, event->entity, DEFENSE);

  ecs_add(gRegistry, event->entity, VITALITY_CHANGED);
  ecs_add(gRegistry, event->entity, AGILITY_CHANGED);
  ecs_add(gRegistry, event->entity, INTELLIGENT_CHANGED);
  ecs_add(gRegistry, event->entity, STRENGTH_CHANGED);
}

static int compute_final_points(const Stat* s)
{
  double totalModifier = (s->modifier[0] + s->modifier[1] + s->modifier[2]) / 100.0;
  int    finalPoints   = s->base * (1.0 + totalModifier);
  return finalPoints;
}

static void vitality(void)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  Health*       health;
  const Stats*  stats;
  int vit;
  ecs_raw(gRegistry, VITALITY_CHANGED, &entities, NULL, &cnt);
  for (int i = cnt - 1; i >= 0; --i)
  {
    health          = ecs_get(gRegistry, entities[i], HEALTH);
    stats           = ecs_get(gRegistry, entities[i], STATS);
    vit             = compute_final_points(&stats->vitality);
    
    health->current = _healthTable[vit] - health->current;
    health->max = _healthTable[vit];

    ecs_rmv(gRegistry, entities[i], VITALITY_CHANGED);
  }
}

static void agility(void)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  Stats*        stats;
  MoveSpeed*    moveSpeed;
  int           points;

  ecs_raw(gRegistry, AGILITY_CHANGED, &entities, NULL, &cnt);
  for (int i = cnt - 1; i >= 0; --i)
  {
    stats            = ecs_get(gRegistry, entities[i], STATS);
    moveSpeed        = ecs_get(gRegistry, entities[i], MOVE_SPEED);
    points           = compute_final_points(&stats->agility);
    moveSpeed->value = points * 10;
    ecs_rmv(gRegistry, entities[i], AGILITY_CHANGED);
  }
}

INLINE int clampi(int _min, int _max, int x)
{
  return min(_max, max(_min, x));
}

static void update_mana(int intelligent, Mana* mana)
{
  mana->current = _manaTable[intelligent] - mana->current;
  mana->max     = _manaTable[intelligent];
}

static void intelligent(void)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  Stats*        stats;
  int           intelligent;

  ecs_raw(gRegistry, INTELLIGENT_CHANGED, &entities, NULL, &cnt);
  for (int i = cnt - 1; i >= 0; --i)
  {
    stats       = ecs_get(gRegistry, entities[i], STATS);
    intelligent = compute_final_points(&stats->intelligent);
    update_mana(intelligent, ecs_get(gRegistry, entities[i], MANA));
    ecs_rmv(gRegistry, entities[i], INTELLIGENT_CHANGED);
  }
}

static void init_health_table()
{
  const int MAX_GROW_RATE    = 4;
  const int SLOW_DOWN_FACTOR = 5;
  int       statValue        = 1;
  _healthTable[statValue]      = statValue + MAX_GROW_RATE;
  for (statValue = 2; statValue < STAT_MAX; ++statValue)
    _healthTable[statValue] =
        _healthTable[statValue - 1] +
        (MAX_GROW_RATE - clampi(0, MAX_GROW_RATE, statValue / SLOW_DOWN_FACTOR));
  for (statValue = 1; statValue < STAT_MAX; ++statValue)
    printf("%dvit: %dhp\n", statValue, _healthTable[statValue]);
}

static void init_mana_table()
{
  const int MAX_GROW_RATE    = 3;
  const int SLOW_DOWN_FACTOR = 5;
  int       statValue        = 1;
  _manaTable[statValue]      = statValue + MAX_GROW_RATE;
  for (statValue = 2; statValue < STAT_MAX; ++statValue)
    _manaTable[statValue] =
        _manaTable[statValue - 1] +
        (MAX_GROW_RATE - clampi(0, MAX_GROW_RATE, statValue / SLOW_DOWN_FACTOR));
  for (statValue = 1; statValue < STAT_MAX; ++statValue)
    printf("%dint -> %dmp\n", statValue, _manaTable[statValue]);
}

void stats_system_init(void)
{
  init_mana_table();
  init_health_table();
  signal_connect(ecs_on_construct(gRegistry, STATS), CALLBACK_2(on_stats_added));
}

void stats_system(void)
{
  vitality();
  intelligent();
  agility();
}
