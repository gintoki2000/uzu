#include "components.h"
#include "entity_utils.h"
#include "resources.h"
#include "session.h"
#include "system/rendering.h"
#include <string.h>

static const SDL_Rect _start = { 0, 0, 4, 9 };

static const SDL_Rect _body = { 4, 0, 3, 9 };

static const SDL_Rect _end = { 7, 0, 5, 9 };

static const SDL_Color _bloodActive = { 237, 19, 41, 255 };

static const SDL_Color _bloodInactive = { 135, 11, 23, 255 };

static const SDL_Color _manaActive = { 87, 152, 203, 255 };

static const SDL_Color _manaInactive = { 96, 47, 86, 255 };

#define HUD_HEALTH_BAR_POSITION_X 32
#define HUD_HEALTH_BAR_POSITION_Y 5
#define HUD_MANA_BAR_POSITION_X 32
#define HUD_MANA_BAR_POSITION_Y 18
#define SPELL_NAME_POSITION_X 5
#define SPELL_NAME_POSITION_Y 40
#define COINS_POSITION_X 5
#define COINS_POSITION_Y 60

extern Ecs*          gEcs;
extern SDL_Renderer* gRenderer;

typedef struct DrawBarParams
{
  u16   length;
  u16   points;
  int   position_x;
  int   position_y;
  COLOR active_color;
  COLOR inactive_color;
} DrawBarParams;

static void draw_bar(const DrawBarParams* params);
static void draw_player_health_bar(ecs_entity_t player);
static void draw_player_mana_bar(ecs_entity_t player);
static void draw_spell_name(ecs_entity_t player);
static void draw_coins(void);
static void draw_spell_icon(ecs_entity_t player);

void hub_rendering_system()
{
  ecs_entity_t player;
  if ((player = scn_get_player(gEcs)) != ECS_NULL_ENT)
  {
    draw_player_health_bar(player);
    draw_player_mana_bar(player);
    draw_spell_icon(player);
    //draw_spell_name(player);
    draw_coins();
  }
}

static void draw_bar(const DrawBarParams* params)
{

  SDL_Texture* texture;
  SDL_Rect     dst;

  dst.x = params->position_x;
  dst.y = params->position_y;
  dst.w = _start.w;
  dst.h = _start.h;

  texture = get_texture(TEX_PLAYER_HEALTH_BAR);

  SDL_RenderCopy(gRenderer, texture, &_start, &dst);

  for (int i = 0; i < params->length; ++i)
  {
    dst.x = params->position_x + _start.w + i * _body.w;
    dst.y = params->position_y;
    dst.w = _body.w;
    dst.h = _body.h;
    SDL_RenderCopy(gRenderer, texture, &_body, &dst);
    SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
  }

  dst.x = params->position_x + _start.w + params->length * _body.w;
  dst.y = params->position_y;
  dst.w = _end.w;
  dst.h = _end.h;
  SDL_RenderCopy(gRenderer, texture, &_end, &dst);

  for (int i = 0; i < params->length; ++i)
  {
    dst.x = params->position_x + (_start.w + 1) + i * 3;
    dst.y = params->position_y + 3;
    dst.w = 2;
    dst.h = 3;
    if (i < params->points)
    {
      SDL_SetRenderDrawColor(gRenderer,
                             params->active_color.r,
                             params->active_color.g,
                             params->active_color.b,
                             params->active_color.a);
    }
    else
    {
      SDL_SetRenderDrawColor(gRenderer,
                             params->inactive_color.r,
                             params->inactive_color.g,
                             params->inactive_color.b,
                             params->inactive_color.a);
    }
    SDL_RenderFillRect(gRenderer, &dst);
  }
}

static void draw_player_health_bar(ecs_entity_t player)
{
  Health* health;

  if ((health = ecs_get(gEcs, player, HEALTH)))
  {
    DrawBarParams params;
    params.position_x     = HUD_HEALTH_BAR_POSITION_X;
    params.position_y     = HUD_HEALTH_BAR_POSITION_Y;
    params.active_color   = _bloodActive;
    params.inactive_color = _bloodInactive;
    params.length         = health->max;
    params.points         = health->current;
    draw_bar(&params);
  }
}

static void draw_player_mana_bar(ecs_entity_t player)
{
  Mana* mana;

  if ((mana = ecs_get(gEcs, player, MANA)))
  {
    DrawBarParams params;
    params.position_x     = HUD_MANA_BAR_POSITION_X;
    params.position_y     = HUD_MANA_BAR_POSITION_Y;
    params.active_color   = _manaActive;
    params.inactive_color = _manaInactive;
    params.length         = mana->max;
    params.points         = mana->current;
    draw_bar(&params);
  }
}

static void draw_coins()
{
  char str_coins[10];
  sprintf(str_coins, "%d", gSession.coins);
  FC_DrawColor(get_font(FONT_DAMAGE_INDICATOR),
               gRenderer,
               COINS_POSITION_X,
               COINS_POSITION_Y,
               (COLOR){ 245, 185, 66, 255 },
               "%s$",
               str_coins);
}

static void draw_spell_icon(ecs_entity_t player)
{
  static const RECT icon_dst  = { 7, 7, 16, 16 };
  static const RECT frame_dst = { 0, 0, 30, 30 };

  AttunementSlot* aslot;
  const Spell*    spell;
  Icon            icon;

  SDL_RenderCopy(gRenderer, get_texture(TEX_SPELL_FRAME), NULL, &frame_dst);

  aslot = ecs_get(gEcs, player, ATTUNEMENT_SLOT);
  if (aslot->spellId == SPELL_ID_NULL)
    return;
  spell = &gSpellTbl[aslot->spellId];
  icon  = spell->icon;

  SDL_RenderCopy(gRenderer, get_texture(icon.textureId), &icon.rect, &icon_dst);
}
