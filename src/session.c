#include "session.h"

#include "SDL.h"
#include "constances.h"
#include "global.h"
#include "inventory.h"

struct Session g_session;

static u16 _starting_weapon_tbl[] = {
  WEAPON_CLEAVER,
  WEAPON_SPEAR,
  WEAPON_STAFF,
  WEAPON_BOW,
};

u16 g_initial_item1_tbl[] = {
  ITEM_TYPE_CLEAVER,
  ITEM_TYPE_SPEAR,
  ITEM_TYPE_STAFF,
  ITEM_TYPE_ID_NULL,
};

const u16 g_initial_item2_tbl[] = {
  ITEM_TYPE_ID_NULL,
  ITEM_TYPE_ID_NULL,
  ITEM_TYPE_SCROLL_FIRE_BALL,
  ITEM_TYPE_ID_NULL,
};

const u16 g_initial_spell[] = {
  SPELL_ID_NULL,
  SPELL_ID_NULL,
  SPELL_FIRE_BALL,
  SPELL_ID_NULL,
};

#define STARTING_POSITION_X (150)
#define STARTING_POSITION_Y (90)

void new_game(u16 starting_job)
{
  g_session.job      = starting_job;
  g_session.new_game = TRUE;
  g_session.pos.x    = STARTING_POSITION_X;
  g_session.pos.y    = STARTING_POSITION_Y;
  g_session.weapon   = _starting_weapon_tbl[starting_job];
  g_session.spell    = g_initial_spell[starting_job];

  inv_add_item(g_initial_item1_tbl[starting_job], 1);
  inv_add_item(g_initial_item2_tbl[starting_job], 1);
  SDL_strlcpy(g_session.level, "demon_ruin", 255);
}
