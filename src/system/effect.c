#include "SDL_stdinc.h"
#include "components.h"
#include "ecs/ecs.h"
#include "entity_factory.h"
#include "entity_utils.h"
#include "global.h"
#include "resources.h"
#include "system/event_messaging_sys.h"
#include "system/game_logic.h"

extern Ecs* g_ecs;

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
  PickupableAttributes* attrs = ecs_get(g_ecs, event->pickupable_entity, PICKUPABLE_ATTRIBUTES);
  u16                   item_type_id = gPickupableToItemTypeIdTbl[attrs->id];
  if (attrs->sfx != SFX_ID_NULL)
    Mix_PlayChannel(-1, get_sfx(attrs->sfx), 0);
  if (item_type_id != ITEM_TYPE_ID_NULL)
    make_fx_item_picked_up(g_ecs, event->position, gItemTypes[item_type_id].name);
}

static ecs_entity_t (*const s_hit_effect_fn_tbl[])(Ecs*, Vec2) = {
  make_fx_blood_loss, make_fx_blood_loss, make_fx_fire_hit, make_fx_blood_loss, make_fx_ice_hit,
};

static COLOR s_damage_type_color_tbl[] = {
  { 0xff, 0x00, 0x00, 0xff }, { 0xff, 0x00, 0x00, 0xff }, { 0xfc, 0xad, 0x03, 0xff },
  { 0xff, 0x00, 0x00, 0xff }, { 0x03, 0xba, 0xfc, 0xff },
};

static u16 s_hit_sfx_tbl[] = {
  SFX_SWORD_HIT, SFX_SWORD_HIT, SFX_SWORD_HIT, SFX_SWORD_HIT, SFX_SWORD_HIT,
};

extern RECT g_viewport;

static void on_get_damaged(void* arg, const GetDamagedMsg* event)
{
  (void)arg;
  Transform* transform;
  HitBox*    hitbox;
  Vec2       particle_position;
  Vec2       damagee_topleft;
  POINT      p;

  transform = ecs_get(g_ecs, event->damagee, TRANSFORM);
  hitbox    = ecs_get(g_ecs, event->damagee, HITBOX);
  if (transform != NULL)
  {
    particle_position.x = transform->position.x;
    particle_position.y = transform->position.y - 30.f;
    make_fx_damage_indicator(g_ecs,
                             particle_position,
                             s_damage_type_color_tbl[event->type],
                             event->damage);

    p.x = transform->position.x;
    p.y = transform->position.y;
    if (SDL_PointInRect(&p, &g_viewport))
      Mix_PlayChannel(-1, get_sfx(s_hit_sfx_tbl[event->type]), 0);
  }

  if (transform != NULL && hitbox != NULL)
  {
    damagee_topleft.x = transform->position.x - hitbox->anchor.x;
    damagee_topleft.y = transform->position.y - hitbox->anchor.y;

    particle_position.x = rand() % ((int)hitbox->size.x) + damagee_topleft.x;
    particle_position.y = rand() % ((int)hitbox->size.y) + damagee_topleft.y;

    s_hit_effect_fn_tbl[event->type](g_ecs, particle_position);
  }
}

static void on_coin_picked_up(void* arg, const CoinPickedUpMsg* event)
{
  Mix_PlayChannel(-1, get_sfx(SFX_COIN), 0);
}
