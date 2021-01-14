#include "session.h"
#include "types.h"

struct Session g_session;

void new_game(u16 starting_class)
{
  g_session.job      = starting_class;
  g_session.hp       = 20;
  g_session.mp       = 20;
  g_session.new_game = TRUE;

  switch (starting_class)
  {
  case JOB_DRAGON:
    g_session.weapon = WEAPON_SPEAR;
    break;
  case JOB_HUNTER:
    g_session.weapon = WEAPON_BOW;
    break;
  case JOB_KNIGHT:
    g_session.weapon = WEAPON_ANIME_SWORD;
    break;
  case JOB_WIZZARD:
    g_session.weapon = WEAPON_STAFF;
    break;
  }
}
