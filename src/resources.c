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
  [TEX_MON_BIG_DEMON] = "res/monster/big_demon.png",
  [TEX_MON_WOGOL]     = "res/monster/wogol.png",
  [TEX_MON_IMP]       = "res/monster/imp.png",
  [TEX_MON_CHORT]     = "res/monster/chort.png",

  /*jobs*/
  [TEX_JOB_KNIGHT]  = "res/job/knight.png",
  [TEX_JOB_LIZZARD] = "res/job/lizzard.png",
  [TEX_JOB_WIZZARD] = "res/job/wizzard.png",
  [TEX_JOB_ELF]     = "res/job/elf_m.png",

  /*weapons*/
  [TEX_WPN_ANIME_SWORD]  = "res/weapon/anime_sword.png",
  [TEX_WPN_AXE]          = "res/weapon/axe.png",
  [TEX_WPN_BOW]          = "res/weapon/bow.png",
  [TEX_WPN_CLEAVER]      = "res/weapon/cleaver.png",
  [TEX_WPN_GOLDEN_SWORD] = "res/weapon/lavish_sword.png",
  [TEX_WPN_KATANA]       = "res/weapon/katana.png",
  [TEX_WPN_RED_STAFF]    = "res/weapon/red_magic_staff.png",
  [TEX_WPN_SPEAR]        = "res/weapon/spear.png",

  /*fx*/
  [TEX_FX_BLOOD]            = "res/effect/blood.png",
  [TEX_FX_BLOOD_1]          = "res/effect/blood_1.png",
  [TEX_FX_BLOOD_2]          = "res/effect/blood_2.png",
  [TEX_FX_FIRE_BUST]        = "res/effect/fire_bust.png",
  [TEX_FX_FIRE_CAST]        = "res/effect/firecast.png",
  [TEX_FX_FIRE_HIT]         = "res/effect/frehit.png",
  [TEX_FX_GOLDEN_CROSS_HIT] = "res/effect/golden_cross_hit.png",
  [TEX_FX_ICE_CAST]         = "res/effect/icecast.png",
  [TEX_FX_ICE_HIT]          = "res/effect/icehit.png",
  [TEX_FX_SLASH]            = "res/effect/slash.png",
  [TEX_FX_HALO_EXPL]        = "res/effect/halo_explosion1.png",
  [TEX_FX_FREEZZED]         = "res/effect/frezzed.png",
  [TEX_FX_POISONED]         = "res/effect/poisoned.png",

  /*spells*/
  [TEX_FIRE_BALL]      = "res/spell/fire_ball.png",
  [TEX_ICE_ARROW]      = "res/spell/ice_arrow.png",
  [TEX_RED_THUNDER]    = "res/spell/red_thunder.png",
  [TEX_YELLOW_THUNDER] = "res/spell/yellow_thunder.png",
  [TEX_PURPLE_BALL]    = "res/spell/purple_ball.png",

  /*items*/
  [TEX_ITEM_FLASK_RED]     = "res/item/flask_red.png",
  [TEX_ITEM_FLASK_RED_BIG] = "res/item/flask_big_red.png",
  [TEX_ITEM_SCROLL]        = "res/item/scroll.png",
  [TEX_ITEM_KEY]           = "res/item/key.png",
  [TEX_ITEM_COIN]          = "res/coin.png",
  [TEX_ITEM_BLUE_FLASK]    = "res/item/flask_blue.png",

  /*chars*/
  [TEX_ELITE_KNIGHT] = "res/character/elite_knight.png",

  [TEX_ARROW]                = "res/weapon/arrow_1.png",
  [TEX_CHEST]                = "res/chest.png",
  [TEX_CURSOR_CROSS]         = "res/crosshair.png",
  [TEX_CURSOR_POINTER]       = "res/cursor_pointer.png",
  [TEX_DOOR]                 = "res/door.png",
  [TEX_EMOJI_BANG]           = "res/emoji/bang.png",
  [TEX_EMOJI_HEART]          = "res/emoji/heart.png",
  [TEX_EMOJI_QUESTION]       = "res/emoji/question.png",
  [TEX_EMOJI_SILENT]         = "res/emoji/silent.png",
  [TEX_ICON_BOW]             = "res/ui/bow.png",
  [TEX_ICON_CLEAVER]         = "res/ui/icon_cleaver.png",
  [TEX_ICON_CONSUMABLE]      = "res/ui/icon_consumable.png",
  [TEX_ICON_FIRE_BALL]       = "res/ui/icon_fire_ball.png",
  [TEX_ICON_ICE_ARROW]       = "res/ui/icon_ice_arrow.png",
  [TEX_ICON_KEY_ITEM]        = "res/ui/key_item.png",
  [TEX_ICON_SPEAR]           = "res/ui/icon_spear.png",
  [TEX_ICON_SPELL]           = "res/ui/icon_spell.png",
  [TEX_ICON_STAFF]           = "res/ui/icon_staff.png",
  [TEX_ICON_WEAPON]          = "res/ui/icon_weapon.png",
  [TEX_INVENTORY]            = "res/ui/inventory.png",
  [TEX_INVENTORY_CURSOR]     = "res/ui/inventory_cursor.png",
  [TEX_INVENTORY_TAB_ACTIVE] = "res/ui/inventory_tab_active.png",
  [TEX_KEYS]                 = "res/ui/keys.png",
  [TEX_PLAYER_HEALTH_BAR]    = "res/ui/player_health_bar.png",
  [TEX_PONITER_DOWN]         = "res/pointer_down.png",
  [TEX_SPELL_FRAME]          = "res/ui/spell_frame.png",
  [TEX_TILESCR_BG]           = "res/tilescreenbg.png",
  [TEX_TILESET]              = "res/titleset.png",
  [TEX_UI_DIALOGUE]          = "res/ui/dialogue.png",
  [TEX_UI_MAIN_MENU]         = "res/ui/main_menu.png",
  [TEX_UI_MAIN_MENU_POINTER] = "res/ui/main_menu_pointer.png",
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
  [CON_BRIAN_FIRST_ENCOUNTER] = "res/conversation/nova_00.json",
  [CON_BRIAN_RESCUE_1]        = "res/conversation/nova_1A_1.json",
  [CON_BRIAN_RESCUE_2]        = "res/conversation/nova_2A_1.json",
  [CON_BRIAN_RESCUE_3]        = "res/conversation/rescue_brian_3.json",
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
    ASSERT(_texture_files[i] != NULL);
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

  for (int i = 0; i < NUM_CONVERSATIONS; ++i)
  {
    struct json_object* json = load_json_from_file(_conversation_files[i]);
    parse_struct(gStructMetaData_Conversation.fields, &_conversations[i], json);
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
