#include "session.h"

#include "constances.h"
#include "global.h"

struct Session g_session;

static u16 _starting_weapon_tbl[] = {
  WEAPON_CLEAVER,
  WEAPON_SPEAR,
  WEAPON_STAFF,
  WEAPON_BOW,
};

u16 g_initial_hp_tbl[] = { 15, 13, 8, 10 };

u16 g_initial_mp_tbl[] = { 5, 6, 12, 6 };

#define STARTING_POSITION_X (TILE_SIZE * 5)
#define STARTING_POSITION_Y (TILE_SIZE * 7)

void new_game(u16 starting_job)
{
  g_session.job      = starting_job;
  g_session.hp       = g_initial_hp_tbl[starting_job];
  g_session.mp       = g_initial_mp_tbl[starting_job];
  g_session.new_game = TRUE;
  g_session.pos.x    = STARTING_POSITION_X;
  g_session.pos.y    = STARTING_POSITION_Y;
  g_session.weapon   = _starting_weapon_tbl[starting_job];

  strcpy(g_session.level, "0");
}
