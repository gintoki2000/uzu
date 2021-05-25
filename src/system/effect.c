#include "SDL_stdinc.h"
#include "components.h"
#include "ecs/ecs.h"
#include "entity_factory.h"
#include "entity_utils.h"
#include "global.h"
#include "resources.h"
#include "system/event_messaging_sys.h"
#include "system/game_logic.h"

extern Ecs* gEcs;
extern RECT gViewport;

//<----------------------------event callback---------------------->//
static void on_item_picked_up(void* arg, const ItemPickedUpMsg* event);
static void on_get_damaged(void* arg, const GetDamagedMsg* event);
static void on_coin_picked_up(void* arg, const CoinPickedUpMsg* event);
//==================================================================//

void effect_system_init()
{
  ems_connect(MSG_ITEM_PICKED_UP, CALLBACK_2(on_item_picked_up));
  ems_connect(MSG_GET_DAMAGED, CALLBACK_2(on_get_damaged));
  ems_connect(MSG_COIN_PICKED_UP, CALLBACK_2(on_coin_picked_up));
}

static void on_item_picked_up(SDL_UNUSED void* arg, const ItemPickedUpMsg* event)
{
  PickupableAttributes* attrs = ecs_get(gEcs, event->pickupable_entity, PICKUPABLE_ATTRIBUTES);
  u16                   itemTypeId = gPickupableToItemTypeIdTbl[attrs->id];
  if (attrs->sfx != SFX_ID_NULL)
    Mix_PlayChannel(-1, get_sfx(attrs->sfx), 0);
  if (itemTypeId != ITEM_TYPE_ID_NULL)
    make_fx_item_picked_up(gEcs, event->position, gItemTypes[itemTypeId].name);
}

static ecs_entity_t (*const _hitEffectFuncTbl[])(Ecs*, Vec2) = {
  make_fx_blood_loss, make_fx_blood_loss, make_fx_fire_hit, make_fx_blood_loss, make_fx_ice_hit,
};

static COLOR _damageTypeColorTbl[] = {
  { 0xff, 0x00, 0x00, 0xff }, { 0xff, 0x00, 0x00, 0xff }, { 0xfc, 0xad, 0x03, 0xff },
  { 0xff, 0x00, 0x00, 0xff }, { 0x03, 0xba, 0xfc, 0xff },
};

static u32 _hitSfxTbl[] = {
  SFX_SWORD_HIT, SFX_SWORD_HIT, SFX_SWORD_HIT, SFX_SWORD_HIT, SFX_SWORD_HIT,
};


static void on_get_damaged(SDL_UNUSED void* arg, const GetDamagedMsg* event)
{
  Transform* transform;
  HitBox*    hitbox;
  Vec2       particlePosition;
  Vec2       damageeTopleft;
  POINT      damageePosition;

  transform = ecs_get(gEcs, event->damagee, TRANSFORM);
  hitbox    = ecs_get(gEcs, event->damagee, HITBOX);
  if (transform != NULL)
  {
    particlePosition.x = transform->position.x;
    particlePosition.y = transform->position.y - 30.f;
    make_fx_damage_indicator(gEcs,
                             particlePosition,
                             _damageTypeColorTbl[event->type],
                             event->damage);

    damageePosition.x = transform->position.x;
    damageePosition.y = transform->position.y;

    BOOL isDammageeInViewport = SDL_PointInRect(&damageePosition, &gViewport);
    if (isDammageeInViewport)
      Mix_PlayChannel(-1, get_sfx(_hitSfxTbl[event->type]), 0);
  }

  if (transform != NULL && hitbox != NULL)
  {
    damageeTopleft.x = transform->position.x - hitbox->anchor.x;
    damageeTopleft.y = transform->position.y - hitbox->anchor.y;

    particlePosition.x = rand() % ((int)hitbox->size.x) + damageeTopleft.x;
    particlePosition.y = rand() % ((int)hitbox->size.y) + damageeTopleft.y;

    _hitEffectFuncTbl[event->type](gEcs, particlePosition);
  }
}

static void on_coin_picked_up(SDL_UNUSED void* arg, SDL_UNUSED const CoinPickedUpMsg* event)
{
  Mix_PlayChannel(-1, get_sfx(SFX_COIN), 0);
}
