#include "system/rendering.h"
#include "components.h"
#include "entity_utils.h"
#include "resources.h"
#include "session.h"
#include <string.h>

static const SDL_Rect _start = { 0, 0, 4, 9 };

static const SDL_Rect _body = { 4, 0, 3, 9 };

static const SDL_Rect _end = { 7, 0, 5, 9 };

static const SDL_Color blood_active = { 237, 19, 41, 255 };

static const SDL_Color blood_inactive = { 135, 11, 23, 255 };

static const SDL_Color mana_active = { 87, 152, 203, 255 };

static const SDL_Color mana_inactive = { 96, 47, 86, 255 };

#define HUD_HEALTH_BAR_POSITION_X 5
#define HUD_HEALTH_BAR_POSITION_Y 5
#define HUD_MANA_BAR_POSITION_X 5
#define HUD_MANA_BAR_POSITION_Y 20
#define SPELL_NAME_POSITION_X 5
#define SPELL_NAME_POSITION_Y 40
#define COINS_POSITION_X 5
#define COINS_POSITION_Y 60

extern Ecs*          g_ecs;
extern SDL_Renderer* g_renderer;

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

void hub_system_update()
{
  ecs_entity_t player;
  if ((player = get_player(g_ecs)) != ECS_NULL_ENT)
  {
    draw_player_health_bar(player);
    draw_player_mana_bar(player);
    draw_spell_name(player);
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

  SDL_RenderCopy(g_renderer, texture, &_start, &dst);

  for (int i = 0; i < params->length; ++i)
  {
    dst.x = params->position_x + _start.w + i * _body.w;
    dst.y = params->position_y;
    dst.w = _body.w;
    dst.h = _body.h;
    SDL_RenderCopy(g_renderer, texture, &_body, &dst);
    SDL_SetRenderDrawColor(g_renderer, 255, 255, 255, 255);
  }

  dst.x = params->position_x + _start.w + params->length * _body.w;
  dst.y = params->position_y;
  dst.w = _end.w;
  dst.h = _end.h;
  SDL_RenderCopy(g_renderer, texture, &_end, &dst);

  for (int i = 0; i < params->length; ++i)
  {
    dst.x = params->position_x + (_start.w + 1) + i * 3;
    dst.y = params->position_y + 3;
    dst.w = 2;
    dst.h = 3;
    if (i < params->points)
    {
      SDL_SetRenderDrawColor(g_renderer,
                             params->active_color.r,
                             params->active_color.g,
                             params->active_color.b,
                             params->active_color.a);
    }
    else
    {
      SDL_SetRenderDrawColor(g_renderer,
                             params->inactive_color.r,
                             params->inactive_color.g,
                             params->inactive_color.b,
                             params->inactive_color.a);
    }
    SDL_RenderFillRect(g_renderer, &dst);
  }
}

static void draw_player_health_bar(ecs_entity_t player)
{
  Health* health;

  if ((health = ecs_get(g_ecs, player, HEALTH)))
  {
    DrawBarParams params;
    params.position_x     = HUD_HEALTH_BAR_POSITION_X;
    params.position_y     = HUD_HEALTH_BAR_POSITION_Y;
    params.active_color   = blood_active;
    params.inactive_color = blood_inactive;
    params.length         = health->max_hit_points;
    params.points         = health->hit_points;
    draw_bar(&params);
  }
}

static void draw_player_mana_bar(ecs_entity_t player)
{
  ManaPool* mana_pool;

  if ((mana_pool = ecs_get(g_ecs, player, MANA_POOL)))
  {
    DrawBarParams params;
    params.position_x     = HUD_MANA_BAR_POSITION_X;
    params.position_y     = HUD_MANA_BAR_POSITION_Y;
    params.active_color   = mana_active;
    params.inactive_color = mana_inactive;
    params.length         = mana_pool->max_mana_points;
    params.points         = mana_pool->mana_points;
    draw_bar(&params);
  }
}

static void draw_spell_name(ecs_entity_t player)
{
  AttunementSlot* slot = ecs_get(g_ecs, player, ATTUNEMENT_SLOT);
  if (slot != NULL && slot->spell_id != SPELL_ID_NULL)
  {
    const Spell* spell = &g_spell_tbl[slot->spell_id];
    FC_Draw(get_font(FONT_DAMAGE_INDICATOR),
            g_renderer,
            SPELL_NAME_POSITION_X,
            SPELL_NAME_POSITION_Y,
            "%s",
            spell->name);
  }
}

static void draw_coins()
{
  char str_coins[10];
  sprintf(str_coins, "%d", g_session.coins);
  FC_DrawColor(get_font(FONT_DAMAGE_INDICATOR),
               g_renderer,
               COINS_POSITION_X,
               COINS_POSITION_Y,
               (COLOR){ 245, 185, 66, 255 },
               "%s$",
               str_coins);
}
