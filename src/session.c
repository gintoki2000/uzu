#include "session.h"
#include "entity_factory.h"
u16 g_weapon;
u16 g_hit_points;
u16 g_mana_points;
u16 g_class;
BOOL g_is_new_game;

void new_game(u16 starting_class)
{
  g_class = starting_class;
  g_hit_points = 20;
  g_mana_points = 20;
  
  switch (starting_class) {
    case CLASS_DRAGON:
      g_weapon = WEAPON_SPEAR;
      break;
    case CLASS_HUNTER:
  }
}


