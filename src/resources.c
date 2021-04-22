#include "resources.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"
#include "engine/engine.h"
#include "engine/keyboard.h"
#include "json_helper.h"
#include "struct_meta_data.h"
static SDL_Texture* _textures[NUM_TEXS];
static const char*  _texture_files[NUM_TEXS] = {
  [TEX_BIG_DEMON]            = "res/big_demon.png",
  [TEX_WOGOL]                = "res/wogol.png",
  [TEX_IMP]                  = "res/imp.png",
  [TEX_KNIGHT]               = "res/knight.png",
  [TEX_AXE]                  = "res/axe.png",
  [TEX_BOW]                  = "res/bow.png",
  [TEX_ANIME_SWORD]          = "res/anime_sword.png",
  [TEX_BLOOD]                = "res/blood.png",
  [TEX_ARROW]                = "res/arrow_1.png",
  [TEX_YELLOW_THUNDER]       = "res/Thunder_Yellow.png",
  [TEX_GOLDEN_SWORD]         = "res/weapon_lavish_sword.png",
  [TEX_GOLDEN_CROSS_HIT]     = "res/golden_cross_hit.png",
  [TEX_TILESET]              = "res/titleset.png",
  [TEX_FLASK_RED]            = "res/flask_red.png",
  [TEX_FLASK_RED_BIG]        = "res/flask_big_red.png",
  [TEX_PLAYER_HEALTH_BAR]    = "res/ui/player_health_bar.png",
  [TEX_CHORT]                = "res/chort.png",
  [TEX_CLEAVER]              = "res/weapon_cleaver.png",
  [TEX_RED_THUNDER]          = "res/Thunder.png",
  [TEX_WIZZARD]              = "res/wizzard.png",
  [TEX_PONITER_DOWN]         = "res/pointer_down.png",
  [TEX_BLUE_FLASK]           = "res/flask_blue.png",
  [TEX_CHEST]                = "res/chest.png",
  [TEX_SWING_EFFECT]         = "res/fx/weapon_anime_sword_effect.png",
  [TEX_RED_STAFF]            = "res/weapon_red_magic_staff.png",
  [TEX_SPEAR]                = "res/weapon_spear.png",
  [TEX_UI_MAIN_MENU]         = "res/ui/main_menu.png",
  [TEX_UI_MAIN_MENU_POINTER] = "res/ui/main_menu_pointer.png",
  [TEX_UI_DIALOGUE]          = "res/ui/dialogue.png",
  [TEX_DOOR]                 = "res/door.png",
  [TEX_EFFECT_BLOOD_1]       = "res/effect/blood_1.png",
  [TEX_EFFECT_BLOOD_2]       = "res/effect/blood_2.png",
  [TEX_LIZZARD]              = "res/lizzard.png",
  [TEX_ICE_ARROW]            = "res/IcePick.png",
  [TEX_EFFECT_ICE_CAST]      = "res/effect/icecast.png",
  [TEX_SCROLL]               = "res/scroll.png",
  [TEX_EFFECT_ICE_HIT]       = "res/effect/icehit.png",
  [TEX_KEY]                  = "res/key.png",
  [TEX_EFFECT_FIRE_HIT]      = "res/effect/frehit.png",
  [TEX_EFFECT_FIRE_CAST]     = "res/effect/firecast.png",
  [TEX_FIRE_BALL]            = "res/fireball.png",
  [TEX_ELITE_KNIGHT]         = "res/elite_knight.png",
  [TEX_EFFECT_SLASH]         = "res/SwordFx.png",
  [TEX_EFFECT_FIRE_BUST]     = "res/effect/fire_bust.png",
  [TEX_COIN]                 = "res/coin.png",
  [TEX_ELF]                  = "res/elf_m.png",
  [TEX_TILESCR_BG]           = "res/tilescreenbg.png",
  [TEX_KEYS]                 = "res/ui/keys.png",
  [TEX_ICON_ICE_ARROW]       = "res/ui/icon_ice_arrow.png",
  [TEX_ICON_FIRE_BALL]       = "res/ui/icon_fire_ball.png",
  [TEX_SPELL_FRAME]          = "res/ui/spell_frame.png",
  [TEX_ICON_CLEAVER]         = "res/ui/icon_cleaver.png",
  [TEX_ICON_SPEAR]           = "res/ui/icon_spear.png",
  [TEX_ICON_STAFF]           = "res/ui/icon_staff.png",
  [TEX_ICON_BOW]             = "res/ui/bow.png",
  [TEX_ICON_CONSUMABLE]      = "res/ui/icon_consumable.png",
  [TEX_ICON_WEAPON]          = "res/ui/icon_weapon.png",
  [TEX_ICON_SPELL]           = "res/ui/icon_spell.png",
  [TEX_ICON_KEY_ITEM]        = "res/ui/key_item.png",
  [TEX_INVENTORY]            = "res/ui/inventory.png",
  [TEX_INVENTORY_TAB_ACTIVE] = "res/ui/inventory_tab_active.png",
  [TEX_INVENTORY_CURSOR]     = "res/ui/inventory_cursor.png",
  [TEX_CURSOR_CROSS]         = "res/crosshair.png",
  [TEX_CURSOR_POINTER]       = "res/cursor_pointer.png",
  [TEX_EMOJI_BANG]           = "res/emoji/bang.png",
  [TEX_EMOJI_QUESTION]       = "res/emoji/question.png",
  [TEX_EMOJI_SILENT]         = "res/emoji/silent.png",
};

static Mix_Music*  _bg_musics[NUM_BG_MUSICS];
static const char* _bg_mus_files[NUM_BG_MUSICS] = {
  [BG_MUS_LV1]         = "res/sound/bg/lv1.wav",
  [BG_MUS_LV2]         = "res/sound/bg/lv2.wav",
  [BG_MUS_BOSS]        = "res/sound/bg/boss.wav",
  [BG_MUS_TILE_SCREEN] = "res/sound/bg/tile_screen.wav",
  [BG_MUS_SELECT_CHAR] = "res/sound/bg/itscomefromthedark.mp3",
};

static Mix_Chunk*  _sfx[NUM_SFXS];
static const char* _sfx_files[NUM_SFXS] = {
  [SFX_SWORD_HIT]       = "res/sound/fx/sword_hit.wav",
  [SFX_THUNDER]         = "res/sound/fx/thunder_2v.wav",
  [SFX_INTERACTION]     = "res/sound/fx/interaction1_0.75v.wav",
  [SFX_CLAW_HIT]        = "res/sound/fx/claw_hit.wav",
  [SFX_BUTTON]          = "res/sound/fx/button1.wav",
  [SFX_ICE_SHOOT]       = "res/sound/fx/ice_shoot.wav",
  [SFX_PUNCH]           = "res/sound/fx/sfx_wpn_punch4.wav",
  [SFX_HIT_THUST]       = "res/sound/fx/claw_hit.wav",
  [SFX_MOV_JUMP]        = "res/sound/fx/sfx_movement_jump1.wav",
  [SFX_WEAPON_SWORD]    = "res/sound/fx/sword sound.wav",
  [SFX_EXPOLOSION]      = "res/sound/fx/8bit_bomb_explosion.wav",
  [SFX_FIRE_BALL_LAUCH] = "res/sound/fx/fire_proj.wav",
  [SFX_COIN]            = "res/sound/fx/sfx_coin_single3.wav",
};

static FONT* _fonts[NUM_FONTS];

static Conversation _conversations[NUM_CONVERSATIONS];
static char*        _conversation_files[NUM_CONVERSATIONS] = {
  [CONVERSATION_NOVA_00]   = "res/conversation/nova_00.json",
  [CONVERSATION_NOVA_1A_1] = "res/conversation/nova_1A_1.json",
  [CONVERSATION_NOVA_2A_1] = "res/conversation/nova_2A_1.json",
};

extern SDL_Renderer* g_renderer;

static SDL_Texture* load_texture(const char* file)
{
  return IMG_LoadTexture(g_renderer, file);
}

BOOL resources_load()
{
  for (int i = 0; i < NUM_TEXS; ++i)
  {
    if ((_textures[i] = load_texture(_texture_files[i])) == NULL)
    {
      ERROR("unable to load texture \"%s\":%s\n", _texture_files[i], IMG_GetError());
      return FALSE;
    }
  }

  for (int i = 0; i < NUM_BG_MUSICS; ++i)
  {
    if ((_bg_musics[i] = Mix_LoadMUS(_bg_mus_files[i])) == NULL)
    {
      ERROR("unable to load music \"%s\": %s\n", _bg_mus_files[i], Mix_GetError());
      return FALSE;
    }
  }

  for (int i = 0; i < NUM_SFXS; ++i)
  {
    if ((_sfx[i] = Mix_LoadWAV(_sfx_files[i])) == NULL)
    {
      ERROR("unable to load sfx \"%s\": %s", _sfx_files[i], Mix_GetError());
      return FALSE;
    }
  }

  FONT* font = FC_CreateFont();

  FC_LoadFont(font,
              g_renderer,
              "res/font/font.TTF",
              8,
              (COLOR){ 255, 255, 255, 255 },
              TTF_STYLE_NORMAL);

  _fonts[FONT_DAMAGE_INDICATOR] = font;

  font = FC_CreateFont();

  FC_LoadFont(font,
              g_renderer,
              "res/font/font.TTF",
              8,
              (COLOR){ 255, 255, 255, 255 },
              TTF_STYLE_NORMAL);

  _fonts[FONT_ITEM_PICKED_UP] = font;

  const FieldMetaData* conversation_fmd =
      STRUCT_META_DATA_TBL[STRUCT_META_DATA_CONVERSATION].fields;
  for (int i = 0; i < NUM_CONVERSATIONS; ++i)
  {
    struct json_object* json = load_json_from_file(_conversation_files[i]);
    parse_struct(conversation_fmd, &_conversations[i], json);
    json_object_put(json);
  }

  return TRUE;
}

void resources_unload()
{
  for (int i = 0; i < NUM_TEXS; ++i)
  {
    if (_textures[i] != NULL)
    {
      SDL_DestroyTexture(_textures[i]);
      _textures[i] = NULL;
    }
  }

  for (int i = 0; i < NUM_BG_MUSICS; ++i)
  {
    if (_bg_musics[i] != NULL)
    {
      Mix_FreeMusic(_bg_musics[i]);
      _bg_musics[i] = NULL;
    }
  }
  for (int i = 0; i < NUM_SFXS; ++i)
  {
    if (_sfx[i] != NULL)
      Mix_FreeChunk(_sfx[i]);
    _sfx[i] = NULL;
  }

  for (int i = 0; i < NUM_FONTS; ++i)
  {
    if (_fonts[i] != NULL)
    {
      FC_FreeFont(_fonts[i]);
      _fonts[i] = NULL;
    }
  }

  for (int i = 0; i < NUM_CONVERSATIONS; ++i)
  {
    conversation_fini(&_conversations[i]);
  }
}

SDL_Texture* get_texture(TextureId id)
{
  return _textures[id];
}

Mix_Music* get_bg_mus(BgMusId id)
{
  return _bg_musics[id];
}

Mix_Chunk* get_sfx(SfxId id)
{
  return _sfx[id];
}

FONT* get_font(FontId id)
{
  return _fonts[id];
}

Conversation* get_conversation(ConversationId id)
{
  return _conversations + id;
}

static Icon _key_icon_tbl[] = {
  [SDL_SCANCODE_A]     = { TEX_KEYS, { 65, 39, 12, 12 } },
  [SDL_SCANCODE_S]     = { TEX_KEYS, { 78, 39, 12, 12 } },
  [SDL_SCANCODE_Q]     = { TEX_KEYS, { 59, 26, 12, 12 } },
  [SDL_SCANCODE_W]     = { TEX_KEYS, { 59, 26, 12, 12 } },
  [SDL_SCANCODE_SPACE] = { TEX_KEYS, { 106, 52, 20, 11 } },
  [SDL_SCANCODE_LEFT]  = { TEX_KEYS, { 182, 52, 12, 12 } },
  [SDL_SCANCODE_RIGHT] = { TEX_KEYS, { 207, 52, 12, 12 } },
  [SDL_SCANCODE_UP]    = { TEX_KEYS, { 195, 39, 12, 12 } },
  [SDL_SCANCODE_DOWN]  = { TEX_KEYS, { 195, 52, 12, 12 } },
  [SDL_SCANCODE_D]     = { TEX_KEYS, { 91, 39, 12, 12 } },
};

Icon get_key_icon(u16 key)
{
  return _key_icon_tbl[button_to_scancode(key)];
}
