#include "constances.h"
#include "global.h"

const COLOR gUIColorBg             = { 0xd2, 0xbd, 0xa7, 0xff };
const COLOR gUIColorBorder         = { 0xa9, 0x8b, 0x79, 0xff };
const COLOR UI_COLOR_BORDER_SELECT = { 0xa8, 0x32, 0xb9, 0xff };
// const COLOR UI_COLOR_BG     = { 0x00, 0x00, 0x00, 0x90 };
// const COLOR UI_COLOR_BORDER = { 0xff, 0xff, 0xff, 0x80 };

const COLOR gUIColorText         = { 0x4a, 0x3d, 0x3c, 0xff };
const COLOR gUIColorTextSelected = { 0x33, 0x7e, 0xc5, 0xff };
const COLOR gColorWhite          = { 0xff, 0xff, 0xff, 0xff };
const COLOR gColorBlack          = { 0x00, 0x00, 0x00, 0xff };

const char gCmdTalk[]             = "TALK";
const char gCmdBuy[]              = "BUY";
const char gCmdSell[]             = "SELL";
const char gCmdOpen[]             = "OPEN";
const char gCmdClose[]            = "CLOSE";
const char gCmdExamine[]          = "EXAMINE";

const char TEXT_WEAPON_SPEAR[]       = "spear";
const char TEXT_WEAPON_LAVIS_SWORD[] = "lavis sword";
const char TEXT_WEAPON_STAFF[]       = "staff";
const char TEXT_WEAPON_CLEAVER[]     = "cleaver";

const RECT gRectOpen        = { 32, 0, 32, 34 };
const RECT gRectClose       = { 0, 0, 32, 34 };
const RECT gRectChestClose  = { 0, 0, 16, 16 };
const RECT gRectChestOpen   = { 32, 0, 16, 16 };

const u16 MASK_PLAYER_WEAPON      = BIT(CATEGORY_ENEMY);
const u16 MASK_ENEMY_WEAPON       = BIT(CATEGORY_PLAYER);
const u16 MASK_INTERACABLE_ENTITY = BIT(CATEGORY_INTERACABLE);
