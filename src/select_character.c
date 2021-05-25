#include "animations.h"
#include "constances.h"
#include "dungeon.h"
#include "engine/keyboard.h"
#include "global.h"
#include "resources.h"
#include "scene.h"
#include "session.h"
#include "toolbox/toolbox.h"

#define CHARACTER_POS_X (WIN_WIDTH / 2 - 16)
#define CHARACTER_POS_Y 50
#define JOB_DESC_POS_X (WIN_WIDTH / 2)
#define JOB_DESC_POS_Y 150
#define BUTTON_LR_POS_Y 70
#define BUTTON_LEFT_POS_X (CHARACTER_POS_X - 20)
#define BUTTON_RIGHT_POS_X (CHARACTER_POS_X + 24)

struct PingPongValue
{
  u8 val;
  u8 min;
  u8 max;
  s8 dir;
};

extern RENDERER* gRenderer;

static const char* _jobdesc[NUM_JOBS] = { "knight job desc",
                                          "lizzard job desc",
                                          "wizzard job desc",
                                          "elf job desc" };

static const Animation* _jobanim[NUM_JOBS] = {
  &g_anims_knight_m[ANIM_STATE_IDLE],
  &g_anims_lizzard_m[ANIM_STATE_IDLE],
  &g_anims_wizzard_m[ANIM_STATE_IDLE],
  &g_anims_elf_m[ANIM_STATE_IDLE],
};

static const RECT _jobbox  = { CHARACTER_POS_X - 4, CHARACTER_POS_Y, 25, 40 };
static const RECT _descbox = { WIN_WIDTH / 2 - 80, 120, 160, 60 };

static const FC_Effect _jobdesc_text_effect = {
  .alignment = FC_ALIGN_CENTER,
  .scale     = { 1.f, 1.f },
  .color     = { 0xff, 0xff, 0xff, 0xd0 },
};

static u16        _job = JOB_KNIGHT;
static FONT*      _font;
static u32        _ticks;
static Mix_Chunk* _interact_fx;
static Mix_Chunk* _back_fx;
static TEXTURE*   _bgimg;
static Icon       _lbtn_icon;
static Icon       _rbtn_icon;

static struct PingPongValue _transparent = {
  .val = 0x7f,
  .min = 0x7f,
  .max = 0xd0,
  .dir = 1,
};

DEFINE_SCENE(select_character);

static void process_input(void*);
static void draw_character(void);
static void draw_jobdesc(void);
static void draw_bg(void);
static void draw_icon(Icon icon, int x, int y);

static u8 ppv_step(struct PingPongValue* pp);


static void on_load()
{
  _font        = get_font(FONT_ITEM_PICKED_UP);
  _back_fx     = get_sfx(SFX_INTERACTION);
  _interact_fx = get_sfx(SFX_BUTTON);
  _bgimg       = get_texture(TEX_TILESCR_BG);
  _lbtn_icon   = get_key_icon(BUTTON_LEFT);
  _rbtn_icon   = get_key_icon(BUTTON_RIGHT);
  _ticks       = 0;
  input_push_state(INPUT_STATE_INST_1(process_input));
}

static void on_unload()
{
  input_pop_state();
}

static void on_event(SDL_UNUSED const SDL_Event* event)
{
}

static void on_update()
{
  ++_ticks;
  draw_bg();
  draw_character();
  draw_jobdesc();
}

static void process_input(SDL_UNUSED void* arg)
{
  if (button_just_pressed(BUTTON_LEFT))
  {
    _job = _job > 0 ? _job - 1 : NUM_JOBS - 1;
    Mix_PlayChannel(-1, _interact_fx, 0);
  }
  if (button_just_pressed(BUTTON_RIGHT))
  {
    Mix_PlayChannel(-1, _interact_fx, 0);
    _job = _job < NUM_JOBS - 1 ? _job + 1 : 0;
  }
  if (button_just_pressed(BUTTON_CANCEL))
  {
    set_scene(&g_main_scene);
    Mix_PlayChannel(-1, _back_fx, 0);
  }
  if (button_just_pressed(BUTTON_INTERACT))
  {
    new_game(_job);
    set_scene(&g_game_scene);
    Mix_PlayChannel(-1, _interact_fx, 0);
  }
}

static void draw_character(void)
{
  int              frame_index;
  TEXTURE*         texture;
  RECT             src, dst;
  const Animation* anim = _jobanim[_job];

  // draw box
  SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xd0);
  SDL_RenderFillRect(gRenderer, &_jobbox);

  frame_index = (_ticks / anim->frameDuration) % anim->count;

  texture = get_texture(anim->textureId);

  src.x = anim->offsetX + frame_index * anim->spriteWidth;
  src.y = anim->offsetY;
  src.w = anim->spriteWidth;
  src.h = anim->spriteHeight;

  dst.x = CHARACTER_POS_X;
  dst.y = CHARACTER_POS_Y;
  dst.w = src.w;
  dst.h = src.h;

  SDL_RenderCopy(gRenderer, texture, &src, &dst);

  draw_icon(_lbtn_icon, BUTTON_LEFT_POS_X, BUTTON_LR_POS_Y);
  draw_icon(_rbtn_icon, BUTTON_RIGHT_POS_X, BUTTON_LR_POS_Y);
}

static void draw_jobdesc()
{
  SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, ppv_step(&_transparent));
  SDL_RenderFillRect(gRenderer, &_descbox);
  FC_DrawBoxEffect(_font, gRenderer, _descbox, _jobdesc_text_effect, _jobdesc[_job]);
}

static void draw_bg(void)
{
  SDL_RenderCopy(gRenderer, _bgimg, NULL, NULL);
}

static u8 ppv_step(struct PingPongValue* ppv)
{
  ppv->val += ppv->dir;
  ppv->val +=
      (ppv->min - ppv->val) * (ppv->val < ppv->min) + (ppv->max - ppv->val) * (ppv->val > ppv->max);
  ppv->dir += 2 * ((ppv->val == ppv->min) - (ppv->val == ppv->max));
  return ppv->val;
}

static void draw_icon(Icon icon, int x, int y)
{
  RECT dst;
  dst.x = x;
  dst.y = y;
  dst.w = icon.rect.w;
  dst.h = icon.rect.h;

  SDL_RenderCopy(gRenderer, get_texture(icon.textureId), &icon.rect, &dst);
}
