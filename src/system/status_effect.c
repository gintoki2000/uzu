#include "components.h"
#include "entity_utils.h"
#include "resources.h"


static void on_status_effect_added(void* arg, const EcsComponentAdded* e);

#define POISIONED_INFLICT_DAMAGE_INTERVAL 120
struct _PoisonedState
{
  u16 timer;
};
static void poisoned_on_apply(ecs_entity_t, void*);
static void poisoned_on_process(ecs_entity_t, void*);
static void poisoned_on_remove(ecs_entity_t, void*);

#define BURNED_INFLICT_DAMAGE_INTERVAL 120
struct _BurnedState
{
  u16 timer;
};
static void burned_on_apply(ecs_entity_t, void*);
static void burned_on_process(ecs_entity_t, void*);
static void burned_on_remove(ecs_entity_t, void*);

static void freezed_on_apply(ecs_entity_t, void*);
static void freezed_on_remove(ecs_entity_t, void*);

/*
static void paralyzed_on_apply(ecs_entity_t, void*);
static void paralyzed_on_remove(ecs_entity_t, void*);
*/

void (*const _apply_func_tbl[NUM_STATUS_EFFECT])(ecs_entity_t, void*) = {
  freezed_on_apply,
  poisoned_on_apply,
  NULL,
  burned_on_apply,
};

void (*const _remove_func_tbl[NUM_STATUS_EFFECT])(ecs_entity_t, void*) = {
  freezed_on_remove,
  poisoned_on_remove,
  NULL,
  burned_on_remove,
};

void (*const _process_func_tbl[NUM_STATUS_EFFECT])(ecs_entity_t, void*) = {
  NULL,
  poisoned_on_process,
  NULL,
  burned_on_process,
};

static const Animation _poisoned_animation = {
  .texture_id     = TEX_FX_POISONED,
  .offset_x       = 0,
  .offset_y       = 32,
  .sprite_width   = 51,
  .sprite_height  = 32,
  .frame_duration = 6,
  .count          = 6,
};

static Sprite _frezzed_sprite = {
  .texture_id = TEX_FX_FREEZZED,
  .rect       = { 0, 0, 51, 51 },
};

extern Ecs*      g_ecs;
extern RENDERER* g_renderer;
extern RECT      g_viewport;

void status_effect_system_init(void)
{
  signal_connect(ecs_on_construct(g_ecs, STATUS_EFFECT), CALLBACK_2(on_status_effect_added));
}

void status_effect_system(void)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  StatusEffect* status_effect;
  ecs_raw(g_ecs, STATUS_EFFECT, &entities, (void*)&status_effect, &cnt);
  for (int i = cnt - 1; i >= 0; --i)
  {
    ++status_effect[i].elapsed;
    if (status_effect[i].duration > 0)
    {
      --status_effect[i].duration;
      if (_process_func_tbl[status_effect[i].type] != NULL)
        _process_func_tbl[status_effect[i].type](entities[i], status_effect[i].state);
    }
    else
    {
      if (_remove_func_tbl[status_effect[i].type] != NULL)
        _remove_func_tbl[status_effect[i].type](entities[i], status_effect[i].state);
      ecs_rmv(g_ecs, entities[i], STATUS_EFFECT);
    }
  }
}

void status_effect_rendering_system(void)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  StatusEffect* status_effect;
  Visual*       visual;
  Sprite        sprite;
  Vec2          position;
  POINT         top_left;
  POINT         center;
  RECT          dst;

  ecs_raw(g_ecs, STATUS_EFFECT, &entities, (void**)&status_effect, &cnt);
  for (int i = cnt - 1; i >= 0; --i)
  {
    switch (status_effect[i].type)
    {
    case STATUS_EFFECT_POISONED:
      sprite = animation_keyframe(&_poisoned_animation, status_effect[i].elapsed);
      break;
    case STATUS_EFFECT_FREEZED:
      sprite = _frezzed_sprite;
    }
    visual     = ecs_get(g_ecs, entities[i], VISUAL);
    position   = ett_get_position(g_ecs, entities[i]);
    top_left.x = position.x - visual->anchor.x;
    top_left.y = position.y - visual->anchor.y;
    center.x   = top_left.x + visual->sprite.rect.w / 2;
    center.y   = top_left.y + visual->sprite.rect.h / 2;
    dst.x      = center.x - sprite.rect.w / 2;
    dst.y      = center.y - sprite.rect.h / 2;
    dst.w      = sprite.rect.w;
    dst.h      = sprite.rect.h;
    if (SDL_HasIntersection(&g_viewport, &dst))
    {
      dst.x -= g_viewport.x;
      dst.y -= g_viewport.y;
      SDL_RenderCopy(g_renderer, get_texture(sprite.texture_id), &sprite.rect, &dst);
    }
  }
}

static void on_status_effect_added(SDL_UNUSED void* arg, const EcsComponentAdded* e)
{
  StatusEffect* status_effect = e->component;
  if (_apply_func_tbl[status_effect->type] != NULL)
    _apply_func_tbl[status_effect->type](e->entity, status_effect->state);
}

static void poisoned_on_apply(ecs_entity_t entity, void* _state)
{
  struct _PoisonedState* state = _state;
  Stats*                 stats = ecs_get(g_ecs, entity, STATS);
  state->timer                 = POISIONED_INFLICT_DAMAGE_INTERVAL;
  stats->agility.modifier[0]   = -50;
  ecs_add(g_ecs, entity, AGILITY_CHANGED);
  printf(ECS_ENT_FMT_PATTERN " poisoned\n", ECS_ENT_FMT_VARS(entity));
}

static void poisoned_on_process(ecs_entity_t entity, void* _state)
{
  struct _PoisonedState* state = _state;
  if (--state->timer == 0)
  {
    Health* health = ecs_get(g_ecs, entity, HEALTH);
    if (health->current > 1)
      health->current -= 1;
    state->timer = POISIONED_INFLICT_DAMAGE_INTERVAL;
  }
}

static void poisoned_on_remove(ecs_entity_t entity, void* _state)
{
  struct _PoisonedState* state = _state;
  Stats*                 stats = ecs_get(g_ecs, entity, STATS);
  state->timer                 = POISIONED_INFLICT_DAMAGE_INTERVAL;
  stats->agility.modifier[0]   = 0;
  ecs_add(g_ecs, entity, AGILITY_CHANGED);
  printf(ECS_ENT_FMT_PATTERN "poisoned was removed\n", ECS_ENT_FMT_VARS(entity));
}

static void burned_on_apply(ecs_entity_t entity, void* _state)
{
  struct _BurnedState* state  = _state;
  Stats*               stats  = ecs_get(g_ecs, entity, STATS);
  state->timer                = POISIONED_INFLICT_DAMAGE_INTERVAL;
  stats->strength.modifier[0] = -20;
  ecs_add(g_ecs, entity, STRENGTH_CHANGED);
  printf(ECS_ENT_FMT_PATTERN " burned\n", ECS_ENT_FMT_VARS(entity));
}

static void burned_on_process(ecs_entity_t entity, void* _state)
{
  struct _BurnedState* state = _state;
  if (--state->timer == 0)
  {
    Health* health = ecs_get(g_ecs, entity, HEALTH);
    if (health->current > 1)
      health->current -= 1;
    state->timer = BURNED_INFLICT_DAMAGE_INTERVAL;
  }
}

static void burned_on_remove(ecs_entity_t entity, void* _state)
{
  struct _BurnedState* state  = _state;
  Stats*               stats  = ecs_get(g_ecs, entity, STATS);
  state->timer                = BURNED_INFLICT_DAMAGE_INTERVAL;
  stats->strength.modifier[0] = 0;
  ecs_add(g_ecs, entity, STRENGTH_CHANGED);
  printf(ECS_ENT_FMT_PATTERN " remove poisoned\n", ECS_ENT_FMT_VARS(entity));
}

static void freezed_on_apply(ecs_entity_t entity, SDL_UNUSED void* state)
{
  ett_unable_to_move_push(g_ecs, entity);
}
static void freezed_on_remove(ecs_entity_t entity, SDL_UNUSED void* state)
{
  ett_unable_to_move_pop(g_ecs, entity);
}
