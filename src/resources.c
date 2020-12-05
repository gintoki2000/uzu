#include "resources.h"
#include <SDL2/SDL_image.h>
#include <engine/engine.h>
static SDL_Texture* _textures[NUM_TEXS];
static const char*  _texture_files[NUM_TEXS] = {
  [TEX_BIG_DEMON] = "asserts/big_demon.png",
  [TEX_KNIGHT] = "asserts/knight.png",
  [TEX_AXE] = "asserts/axe.png",
  [TEX_ANIME_SWORD] = "asserts/anime_sword.png",
  [TEX_BLOOD] = "asserts/blood.png",
  [TEX_BOW] = "asserts/bow.png",
  [TEX_ARROW] = "asserts/arrow.png",
  [TEX_YELLOW_THUNDER] = "asserts/Thunder_Yellow.png",
  [TEX_GOLDEN_SWORD] = "asserts/weapon_lavish_sword.png",
  [TEX_GOLDEN_CROSS_HIT] = "asserts/golden_cross_hit.png",
  [TEX_TILESET] = "asserts/titleset.png",
  [TEX_FLASK_RED] = "asserts/flask_red.png",
  [TEX_FLASK_RED_BIG] = "asserts/flask_big_red.png",
};

static SDL_Texture* load_texture(const char* file)
{
  return IMG_LoadTexture(engine_get_renderer(), file);
}

BOOL resources_load()
{
  for (int i = 0; i < NUM_TEXS; ++i)
  {
    if ((_textures[i] = load_texture(_texture_files[i])) == NULL)
    {
      ERROR("unable to load texture \"%s\"\n", _texture_files[i]);
      return FALSE;
    }
  }
  return TRUE;
}

void resources_unload()
{
  for (int i = 0; i < NUM_TEXS; ++i)
  {
    SDL_DestroyTexture(_textures[i]);
    _textures[i] = NULL;
  }
}

SDL_Texture* get_texture(int texture_id) { return _textures[texture_id]; }
