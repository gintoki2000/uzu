#include "action.h"
#include "components.h"

typedef struct ExpressEmotionAction
{
  Action parent_instance;
  int    emoji;
  int    time;
  BOOL   finished;
} ExpressEmotionAction;

extern ecs_Registry* gRegistry;

static void on_emoji_time_out(ExpressEmotionAction* self, SDL_UNUSED const void* data)
{
  self->finished = TRUE;
}

static void start(void* _self, ecs_entity_t target)
{
  ExpressEmotionAction* self = _self;
  ecs_set(gRegistry, target, EMOJI, &(Emoji){ self->emoji, self->time, { self, on_emoji_time_out } });
}

static BOOL update(void* _self, SDL_UNUSED ecs_entity_t target)
{
  ExpressEmotionAction* self = _self;
  return self->finished;
}

static ActionVtbl _express_emotion_action_vtbl = {
  .start   = start,
  .end     = action_on_end,
  .update  = update,
  .cleanup = action_on_cleanup,
};

Action* express_emotion_action_new(int emoji, int time)
{
  ExpressEmotionAction* obj = action_alloc(ExpressEmotionAction, &_express_emotion_action_vtbl);

  obj->emoji    = emoji;
  obj->finished = FALSE;
  obj->time     = time;

  return (Action*)obj;
}
