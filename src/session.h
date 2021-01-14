#ifndef SESSION_H
#define SESSION_H
#include <toolbox/toolbox.h>

struct Session
{
  u16  job;
  u16  weapon;
  u16  hp;
  u16  mp;
  u16  new_game;
  Vec2 pos;
  char level[255];
};

extern struct Session g_session;

void new_game(u16 starting_class);
#endif // SESSION_H
