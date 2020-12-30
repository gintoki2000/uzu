#include "interaction_system.h"
#include "collision_sys.h"
#include "mediator.h"
#include <components.h>
#include <constances.h>
#include <ecs/ecs.h>
#include <resources.h>
#include <utils.h>

#define INTERACTABLE_DISTANCE TILE_SIZE * 4
#define POINTER_DOWN_WIDTH 6
#define POINTER_DOWN_HEIGHT 3

ecs_entity_t g_curr_iteractable_entity = ECS_NULL_ENT;

extern Ecs*          g_ecs;
extern SDL_Renderer* g_renderer;
extern SDL_Rect      g_viewport;

static ecs_entity_t get_closest_interactable_entity()
{

  return ECS_NULL_ENT;
}

static void update_pointed_entity()
{
  if (g_curr_iteractable_entity == ECS_NULL_ENT)
  {
    Vec2         player_pos, ientity_pos;
    RECT         rect;
    ecs_entity_t entities[5];
    u16          cnt;
    float        closest_distance;
    float        dist;
    ecs_entity_t closest;

    player_pos = get_player_position(g_ecs);
    rect.x     = player_pos.x - TILE_SIZE * 2;
    rect.y     = player_pos.y - TILE_SIZE * 2;
    rect.w     = TILE_SIZE * 4;
    rect.h     = TILE_SIZE * 4;

    collision_system_query_ex(&rect, BIT(CATEGORY_INTERACABLE), entities, &cnt, 5);

    if (cnt > 0)
    {
      closest          = entities[0];
      closest_distance = vec2_mag(vec2_sub(player_pos, get_entity_position(g_ecs, entities[0])));
      for (int i = 1; i < cnt; ++i)
      {
        ientity_pos = get_entity_position(g_ecs, entities[i]);
        if ((dist = vec2_mag(vec2_sub(ientity_pos, player_pos))) < closest_distance)
        {
          closest_distance = dist;
          closest          = entities[i];
        }
      }
      g_curr_iteractable_entity = closest;
    }
  }
  else
  {
    Vec2 player_pos, ientity_pos;

    player_pos  = get_player_position(g_ecs);
    ientity_pos = get_entity_position(g_ecs, g_curr_iteractable_entity);

    if (vec2_mag(vec2_sub(player_pos, ientity_pos)) > INTERACTABLE_DISTANCE)
    {
      g_curr_iteractable_entity = ECS_NULL_ENT;
    }
  }
}

static void on_begin_interaction(void* arg, const SysEvt_BeginInteraction* event)
{
  (void)arg;
  INFO("begin interact with e" ECS_ENT_FMT_PATTERN "\n", ECS_ENT_FMT_VARS(event->entity));
}

void interaction_system_init()
{
  mediator_connect(SYS_SIG_BEGIN_INTERACTION, NULL, SLOT(on_begin_interaction));
}
void interaction_system_update()
{
  update_pointed_entity();
}

void interactable_pointer_rendering_system_update()
{
  if (g_curr_iteractable_entity != ECS_NULL_ENT)
  {
    Vec2 pos = get_entity_position(g_ecs, g_curr_iteractable_entity);

    SDL_RenderCopy(g_renderer,
                   get_texture(TEX_PONITER_DOWN),
                   NULL,
                   &(RECT){
                       pos.x - g_viewport.x - POINTER_DOWN_WIDTH / 2,
                       pos.y - 2 * TILE_SIZE - g_viewport.y,
                       POINTER_DOWN_WIDTH,
                       POINTER_DOWN_HEIGHT,
                   });
  }
}
