#include "resources.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"
#include "json_helper.h"
#include "struct_meta_data.h"
#include <engine/engine.h>
static SDL_Texture* _textures[NUM_TEXS];
static const char*  _texture_files[NUM_TEXS] = {
  [TEX_BIG_DEMON]            = "asserts/big_demon.png",
  [TEX_WOGOL]                = "asserts/wogol.png",
  [TEX_IMP]                  = "asserts/imp.png",
  [TEX_KNIGHT]               = "asserts/knight.png",
  [TEX_AXE]                  = "asserts/axe.png",
  [TEX_ANIME_SWORD]          = "asserts/anime_sword.png",
  [TEX_BLOOD]                = "asserts/blood.png",
  [TEX_BOW]                  = "asserts/bow.png",
  [TEX_ARROW]                = "asserts/arrow.png",
  [TEX_YELLOW_THUNDER]       = "asserts/Thunder_Yellow.png",
  [TEX_GOLDEN_SWORD]         = "asserts/weapon_lavish_sword.png",
  [TEX_GOLDEN_CROSS_HIT]     = "asserts/golden_cross_hit.png",
  [TEX_TILESET]              = "asserts/titleset.png",
  [TEX_FLASK_RED]            = "asserts/flask_red.png",
  [TEX_FLASK_RED_BIG]        = "asserts/flask_big_red.png",
  [TEX_PLAYER_HEALTH_BAR]    = "asserts/player_health_bar.png",
  [TEX_CHORT]                = "asserts/chort.png",
  [TEX_CLEAVER]              = "asserts/weapon_cleaver.png",
  [TEX_RED_THUNDER]          = "asserts/Thunder.png",
  [TEX_WIZZARD]              = "asserts/wizzard.png",
  [TEX_PONITER_DOWN]         = "asserts/pointer_down.png",
  [TEX_BLUE_FLASK]           = "asserts/flask_blue.png",
  [TEX_CHEST]                = "asserts/chest.png",
  [TEX_SWING_EFFECT]         = "asserts/fx/weapon_anime_sword_effect.png",
  [TEX_RED_STAFF]            = "asserts/weapon_red_magic_staff.png",
  [TEX_SPEAR]                = "asserts/weapon_spear.png",
  [TEX_UI_MAIN_MENU]         = "asserts/ui/main_menu.png",
  [TEX_UI_MAIN_MENU_POINTER] = "asserts/ui/main_menu_pointer.png",
  [TEX_UI_DIALOGUE]          = "asserts/ui/dialogue.png",
  [TEX_DOOR]                 = "asserts/door.png",
  [TEX_LIZZARD]              = "asserts/lizzard.png",
};

static Mix_Music*  _bg_musics[NUM_BG_MUSICS];
static const char* _bg_mus_files[NUM_BG_MUSICS] = {
  [BG_MUS_AROUND_THE_BEND]            = "asserts/sound/bg/Around the Bend.WAV",
  [BG_MUS_BEHIND_THE_COLUMN]          = "asserts/sound/bg/Behind the column.WAV",
  [BG_MUS_CATCH_THE_MYSTERY]          = "asserts/sound/bg/Catch The Mystery.WAV",
  [BG_MUS_DARK_INTENSIONS]            = "asserts/sound/bg/Dark Intentions.WAV",
  [BG_MUS_FANCY_CAKES]                = "asserts/sound/bg/Fancy Cakes.WAV",
  [BG_MUS_IN_DENFESE_OF_MY_STUFF]     = "asserts/sound/bg/Fancy Cakes.WAV",
  [BG_MUS_IN_EARLY_TIME]              = "asserts/sound/bg/In early time.WAV",
  [BG_MUS_INSIDE_DREAM]               = "asserts/sound/bg/Inside Dreams.WAV",
  [BG_MUS_IN_BACK_POCKET]             = "asserts/sound/bg/In the back pocket_100 BPM.WAV",
  [BG_MUS_IN_THE_CHAMP_ELYSEES]       = "asserts/sound/bg/In The Champ Elysees.WAV",
  [BG_MUS_IN_THE_DEPTHS_OF_CAVE]      = "asserts/sound/bg/In the depths of the cave.WAV",
  [BG_MUS_IN_THE_SPINNING]            = "asserts/sound/bg/In the spinning world.WAV",
  [BG_MUS_KILLING_FLIES]              = "asserts/sound/bg/Killing Flies.WAV",
  [BG_MUS_THE_CARANE_DANCE]           = "asserts/sound/bg/The Crane Dance.WAV",
  [BG_MUS_THE_ESSENSE_OF_GOOD_THINGS] = "asserts/sound/bg/The essence of good things.WAV",
  [BG_MUS_UNDER_HOT_SUN]              = "asserts/sound/bg/Under the hot sun.WAV",
  [BG_MUS_WAITING_FOR_EVENT]          = "asserts/sound/bg/Waiting For Events.WAV",
  [BG_MUS_WITH_DIRTY_HAND]            = "asserts/sound/bg/With Dirty Hands.WAV",
  [BG_MUS_WITH_LOOSE_CORDS]           = "asserts/sound/bg/With loose cords.WAV",
  [BG_MUS_WITH_TORN_PANTS]            = "asserts/sound/bg/With torn pants.WAV",
};

static Mix_Chunk*  _sfx[NUM_SFXS];
static const char* _sfx_files[NUM_SFXS] = {
  [SFX_SWORD_HIT]   = "asserts/sound/fx/sword_hit.wav",
  [SFX_THUNDER]     = "asserts/sound/fx/thunder_2v.wav",
  [SFX_INTERACTION] = "asserts/sound/fx/interaction1_0.75v.wav",
  [SFX_CLAW_HIT]    = "asserts/sound/fx/claw_hit.wav",
  [SFX_BUTTON]      = "asserts/sound/fx/button1.wav",
};

static FONT* _fonts[NUM_FONTS];

static Conversation _conversations[NUM_CONVERSATIONS];
static char*        _conversation_files[NUM_CONVERSATIONS] = {
  [CONVERSATION_DEMO1] = "asserts/conversation/demo1.json",
  [CONVERSATION_DEMO2] = "asserts/conversation/demo2.json",
  [CONVERSATION_DEMO3] = "asserts/conversation/demo3.json",
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
              "asserts/font/font.TTF",
              8,
              (COLOR){ 255, 255, 255, 255 },
              TTF_STYLE_NORMAL);

  _fonts[FONT_DAMAGE_INDICATOR] = font;

  font = FC_CreateFont();

  FC_LoadFont(font,
              g_renderer,
              "asserts/font/font.TTF",
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
