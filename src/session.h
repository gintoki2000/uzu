#ifndef SESSION_H
#define SESSION_H
#include <toolbox/toolbox.h>
extern u16  g_weapon;
extern u16  g_hit_points;
extern u16  g_mana_points;
extern u16  g_class;
extern BOOL g_is_new_game;

void new_game(u16 starting_class);
void load_session(u8 slot);
void save_session(u8 slot);
#endif // SESSION_H
