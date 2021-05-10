#include "session.h"

#include "SDL.h"
#include "constances.h"
#include "global.h"
#include "inventory.h"

struct Session gSession;

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
  gSession.job      = starting_job;
  gSession.new_game = TRUE;
  gSession.pos.x    = STARTING_POSITION_X;
  gSession.pos.y    = STARTING_POSITION_Y;
  gSession.weapon   = _starting_weapon_tbl[starting_job];
  gSession.spell    = g_initial_spell[starting_job];

  inv_add_item(g_initial_item1_tbl[starting_job], 1);
  inv_add_item(g_initial_item2_tbl[starting_job], 1);
  SDL_strlcpy(gSession.level, "demon_ruin", 255);
}
