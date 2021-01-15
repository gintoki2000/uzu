#include "session.h"
#include "constances.h"
#include "types.h"

struct Session g_session;

void new_game(u16 starting_job)
{
  g_session.job      = starting_job;
  g_session.hp       = 40;
  g_session.mp       = 40;
  g_session.new_game = TRUE;
  g_session.pos.x    = TILE_SIZE * 5;
  g_session.pos.y    = TILE_SIZE * 7;

  switch (starting_job)
  {
  case JOB_DRAGON:
    g_session.weapon = WEAPON_SPEAR;
    break;
  case JOB_HUNTER:
    g_session.weapon = WEAPON_BOW;
    break;
  case JOB_KNIGHT:
    g_session.weapon = WEAPON_LAVIS_SWORD;
    break;
  case JOB_WIZZARD:
    g_session.weapon = WEAPON_STAFF;
    break;
  }
}
