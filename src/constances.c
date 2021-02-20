#include "constances.h"
#include "global.h"

const COLOR UI_COLOR_BG            = { 0xd2, 0xbd, 0xa7, 0xff };
const COLOR UI_COLOR_BORDER        = { 0xa9, 0x8b, 0x79, 0xff };
const COLOR UI_COLOR_BORDER_SELECT = { 0xa8, 0x32, 0xb9, 0xff };
//const COLOR UI_COLOR_BG     = { 0x00, 0x00, 0x00, 0x90 };
//const COLOR UI_COLOR_BORDER = { 0xff, 0xff, 0xff, 0x80 };

const COLOR UI_COLOR_TEXT          = { 0x4a, 0x3d, 0x3c, 0xff };
const COLOR UI_COLOR_TEXT_SELECT   = { 0x06, 0x18, 0xba, 0xff };
const COLOR COLOR_WHITE            = { 0xff, 0xff, 0xff, 0xff };
const COLOR COLOR_BLACK            = { 0x00, 0x00, 0x00, 0xff };

const char TEXT_COMMAND_TALK[]    = "TALK";
const char TEXT_COMMAND_BUY[]     = "BUY";
const char TEXT_COMMAND_SELL[]    = "SELL";
const char TEXT_COMMAND_OPEN[]    = "OPEN";
const char TEXT_COMMAND_CLOSE[]   = "CLOSE";
const char TEXT_COMMAND_EXAMINE[] = "EXAMINE";

const char TEXT_WEAPON_SPEAR[]       = "spear";
const char TEXT_WEAPON_LAVIS_SWORD[] = "lavis sword";
const char TEXT_WEAPON_STAFF[]       = "staff";
const char TEXT_WEAPON_CLEAVER[]     = "cleaver";

const RECT RECT_DOOR_OPEN   = { 32, 0, 32, 34 };
const RECT RECT_DOOR_CLOSE  = { 0, 0, 32, 34 };
const RECT RECT_CHEST_CLOSE = { 0, 0, 16, 16 };
const RECT RECT_CHEST_OPEN  = { 32, 0, 16, 16 };

const u16 MASK_PLAYER_WEAPON      = BIT(CATEGORY_ENEMY);
const u16 MASK_ENEMY_WEAPON       = BIT(CATEGORY_PLAYER);
const u16 MASK_INTERACABLE_ENTITY = BIT(CATEGORY_INTERACABLE);
