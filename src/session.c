#include "session.h"

#include "global.h"
#include "constances.h"

struct Session g_session;

static u16 s_starting_weapon_tbl[] = {
  WEAPON_CLEAVER,
  WEAPON_SPEAR,
  WEAPON_STAFF,
  WEAPON_BOW,
};

void new_game(u16 starting_job)
{
  g_session.job      = starting_job;
  g_session.hp       = 12;
  g_session.mp       = 10;
  g_session.new_game = TRUE;
  g_session.pos.x    = TILE_SIZE * 5;
  g_session.pos.y    = TILE_SIZE * 7;
  g_session.weapon   = s_starting_weapon_tbl[starting_job]; 

  strcpy(g_session.level, "0");

}
