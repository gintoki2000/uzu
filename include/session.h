#ifndef SESSION_H
#define SESSION_H
#include "toolbox/toolbox.h"

struct Session
{
  u16  job;
  u16  weapon;
  u16  new_game;
  u16  spell;
  u16  coins;
  Vec2 pos;
  char level[256];

  u8 vitality;
  u8 strength;
  u8 intelligent;
  u8 agility;
};

extern struct Session gSession;

void new_game(u16 starting_class);
void save_game_data();
void load_game_data();
#endif // SESSION_H
