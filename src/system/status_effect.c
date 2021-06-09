#include "components.h"
#include "entity_utils.h"
#include "resources.h"

static void on_status_effect_added(void* arg, const ecs_ComponentAdded* e);

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

void (*const _applyFuncTbl[NUM_STATUS_EFFECT])(ecs_entity_t, void*) = {
  freezed_on_apply,
  poisoned_on_apply,
  NULL,
  burned_on_apply,
};

void (*const _removeFuncTbl[NUM_STATUS_EFFECT])(ecs_entity_t, void*) = {
  freezed_on_remove,
  poisoned_on_remove,
  NULL,
  burned_on_remove,
};

void (*const _processFuncTbl[NUM_STATUS_EFFECT])(ecs_entity_t, void*) = {
  NULL,
  poisoned_on_process,
  NULL,
  burned_on_process,
};

static const Animation _poisonedAnimation = {
  .textureId     = TEX_FX_POISONED,
  .offsetX       = 0,
  .offsetY       = 32,
  .spriteWidth   = 51,
  .spriteHeight  = 32,
  .frameDuration = 6,
  .count         = 6,
};

static Sprite _frezzedSprite = {
  .textureId = TEX_FX_FREEZZED,
  .rect       = { 0, 0, 51, 51 },
};

extern ecs_Registry*      gRegistry;
extern RENDERER* gRenderer;
extern RECT      gViewport;

void status_effect_system_init(void)
{
  signal_connect(ecs_on_construct(gRegistry, STATUS_EFFECT), CALLBACK_2(on_status_effect_added));
}

void status_effect_system(void)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  StatusEffect* statusEffect;
  ecs_raw(gRegistry, STATUS_EFFECT, &entities, (void*)&statusEffect, &cnt);
  for (int i = cnt - 1; i >= 0; --i)
  {
    ++statusEffect[i].elapsed;
    if (statusEffect[i].duration > 0)
    {
      --statusEffect[i].duration;
      if (_processFuncTbl[statusEffect[i].type] != NULL)
        _processFuncTbl[statusEffect[i].type](entities[i], statusEffect[i].state);
    }
    else
    {
      if (_removeFuncTbl[statusEffect[i].type] != NULL)
        _removeFuncTbl[statusEffect[i].type](entities[i], statusEffect[i].state);
      ecs_rmv(gRegistry, entities[i], STATUS_EFFECT);
    }
  }
}

void status_effect_rendering_system(void)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  StatusEffect* statusEffect;
  Visual*       visual;
  Sprite        sprite;
  Vec2          position;
  POINT         top_left;
  POINT         center;
  RECT          dst;

  ecs_raw(gRegistry, STATUS_EFFECT, &entities, (void**)&statusEffect, &cnt);
  for (int i = cnt - 1; i >= 0; --i)
  {
    switch (statusEffect[i].type)
    {
    case STATUS_EFFECT_POISONED:
      sprite = animation_keyframe(&_poisonedAnimation, statusEffect[i].elapsed);
      break;
    case STATUS_EFFECT_FREEZED:
      sprite = _frezzedSprite;
    }
    visual     = ecs_get(gRegistry, entities[i], VISUAL);
    position   = ett_get_position(gRegistry, entities[i]);
    top_left.x = (int)position.x - visual->anchor.x;
    top_left.y = (int)position.y - visual->anchor.y;
    center.x   = top_left.x + visual->sprite.rect.w / 2;
    center.y   = top_left.y + visual->sprite.rect.h / 2;
    dst.x      = center.x - sprite.rect.w / 2;
    dst.y      = center.y - sprite.rect.h / 2;
    dst.w      = sprite.rect.w;
    dst.h      = sprite.rect.h;
    if (SDL_HasIntersection(&gViewport, &dst))
    {
      dst.x -= gViewport.x;
      dst.y -= gViewport.y;
      SDL_RenderCopy(gRenderer, get_texture(sprite.textureId), &sprite.rect, &dst);
    }
  }
}

static void on_status_effect_added(SDL_UNUSED void* arg, const ecs_ComponentAdded* e)
{
  StatusEffect* statusEffect = e->component;
  if (_applyFuncTbl[statusEffect->type] != NULL)
    _applyFuncTbl[statusEffect->type](e->entity, statusEffect->state);
}

static void poisoned_on_apply(ecs_entity_t entity, void* _state)
{
  struct _PoisonedState* state = _state;
  Stats*                 stats = ecs_get(gRegistry, entity, STATS);
  state->timer                 = POISIONED_INFLICT_DAMAGE_INTERVAL;
  stats->agility.modifier[0]   = -50;
  ecs_add(gRegistry, entity, AGILITY_CHANGED);
  printf(ECS_ENT_FMT_PATTERN " poisoned\n", ECS_ENT_FMT_VARS(entity));
}

static void poisoned_on_process(ecs_entity_t entity, void* _state)
{
  struct _PoisonedState* state = _state;
  if (--state->timer == 0)
  {
    Health* health = ecs_get(gRegistry, entity, HEALTH);
    if (health->current > 1)
      health->current -= 1;
    state->timer = POISIONED_INFLICT_DAMAGE_INTERVAL;
  }
}

static void poisoned_on_remove(ecs_entity_t entity, void* _state)
{
  struct _PoisonedState* state = _state;
  Stats*                 stats = ecs_get(gRegistry, entity, STATS);
  state->timer                 = POISIONED_INFLICT_DAMAGE_INTERVAL;
  stats->agility.modifier[0]   = 0;
  ecs_add(gRegistry, entity, AGILITY_CHANGED);
  printf(ECS_ENT_FMT_PATTERN "poisoned was removed\n", ECS_ENT_FMT_VARS(entity));
}

static void burned_on_apply(ecs_entity_t entity, void* _state)
{
  struct _BurnedState* state  = _state;
  Stats*               stats  = ecs_get(gRegistry, entity, STATS);
  state->timer                = POISIONED_INFLICT_DAMAGE_INTERVAL;
  stats->strength.modifier[0] = -20;
  ecs_add(gRegistry, entity, STRENGTH_CHANGED);
  printf(ECS_ENT_FMT_PATTERN " burned\n", ECS_ENT_FMT_VARS(entity));
}

static void burned_on_process(ecs_entity_t entity, void* _state)
{
  struct _BurnedState* state = _state;
  if (--state->timer == 0)
  {
    Health* health = ecs_get(gRegistry, entity, HEALTH);
    if (health->current > 1)
      health->current -= 1;
    state->timer = BURNED_INFLICT_DAMAGE_INTERVAL;
  }
}

static void burned_on_remove(ecs_entity_t entity, void* _state)
{
  struct _BurnedState* state  = _state;
  Stats*               stats  = ecs_get(gRegistry, entity, STATS);
  state->timer                = BURNED_INFLICT_DAMAGE_INTERVAL;
  stats->strength.modifier[0] = 0;
  ecs_add(gRegistry, entity, STRENGTH_CHANGED);
  printf(ECS_ENT_FMT_PATTERN " remove poisoned\n", ECS_ENT_FMT_VARS(entity));
}

static void freezed_on_apply(ecs_entity_t entity, SDL_UNUSED void* state)
{
  ett_unable_to_move_push(gRegistry, entity);
}
static void freezed_on_remove(ecs_entity_t entity, SDL_UNUSED void* state)
{
  ett_unable_to_move_pop(gRegistry, entity);
}
