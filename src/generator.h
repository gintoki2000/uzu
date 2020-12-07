#ifndef GENERATOR_H
#define GENERATOR_H
#include "config.h"
#include <ecs/ecs.h>
#include <SDL2/SDL.h>

typedef enum
{
  GENERATOR_PARAM_WIDTH,
  GENERATOR_PARAM_HEIGHT,
  GENERATOR_PARAM_MAX_ROOM_SIZE,
  GENERATOR_PARAM_MIN_ROOM_SIZE
} GeneratorParam;
void set_generator_param(GeneratorParam param, int value);
void generate_new_dungeon(Ecs* ecs);

#ifdef DEBUG
void draw_rooms(SDL_Renderer*, int s);
void draw_graph(SDL_Renderer*, int s);
void draw_tree(SDL_Renderer*, int s);
#endif // DEBUG
#endif // GENERATOR_H
