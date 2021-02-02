#ifndef CONSTANCES_H
#define CONSTANCES_H

#include "toolbox/toolbox.h"

#define TILE_SIZE 16
#define WIN_WIDTH 320
#define WIN_HEIGHT 240
#define SCL_X 4.0
#define SCL_Y 4.0

extern const COLOR UI_COLOR_BG;
extern const COLOR UI_COLOR_TEXT;
extern const COLOR UI_COLOR_BORDER;
extern const COLOR UI_COLOR_BORDER_SELECT;
extern const COLOR UI_COLOR_TEXT_SELECT;

extern const COLOR COLOR_BLACK;
extern const COLOR COLOR_WHITE;

extern const char TEXT_COMMAND_TALK[];
extern const char TEXT_COMMAND_SELL[];
extern const char TEXT_COMMAND_BUY[];
extern const char TEXT_COMMAND_EXAMINE[];
extern const char TEXT_COMMAND_OPEN[];
extern const char TEXT_COMMAND_CLOSE[];

extern const char TEXT_WEAPON_SPEAR[];
extern const char TEXT_WEAPON_LAVIS_SWORD[];
extern const char TEXT_WEAPON_STAFF[];
extern const char TEXT_WEAPON_CLEAVER[];

extern const RECT RECT_DOOR_CLOSE;
extern const RECT RECT_DOOR_OPEN;
extern const RECT RECT_CHEST_CLOSE;
extern const RECT RECT_CHEST_OPEN;

extern const u16 MASK_PLAYER_WEAPON;
extern const u16 MASK_ENEMY_WEAPON;
extern const u16 MASK_INTERACABLE_ENTITY;

#endif // CONSTANCES_H
