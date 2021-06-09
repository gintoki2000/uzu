#include "system/game_logic.h"

#include "entity_utils.h"
#include "system/collision_sys.h"
#include "components.h"
#include "constances.h"
#include "ecs/ecs.h"
#include "resources.h"
#include "ui_list.h"
#include "system/event_messaging_sys.h"

#define INTERACTABLE_DISTANCE TILE_SIZE * 4
#define POINTER_DOWN_WIDTH 6
#define POINTER_DOWN_HEIGHT 3

extern ecs_entity_t  gCurrentInteractingEntity;
extern ecs_Registry*          gRegistry;
extern RECT          gViewport;
extern SDL_Renderer* gRenderer;

void interactable_rendering_system()
{
  if (gCurrentInteractingEntity != ECS_NULL_ENT)
  {
    Vec2 position = ett_get_position(gRegistry, gCurrentInteractingEntity);

    SDL_RenderCopy(gRenderer,
                   get_texture(TEX_PONITER_DOWN),
                   NULL,
                   &(RECT){
                       position.x - gViewport.x - POINTER_DOWN_WIDTH / 2,
                       position.y - 2 * TILE_SIZE - gViewport.y,
                       POINTER_DOWN_WIDTH,
                       POINTER_DOWN_HEIGHT,
                   });
  }
}
