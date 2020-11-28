#ifndef COMPONENTS_H
#define COMPONENTS_H
#include <common.h>
#include <ecs/world.h>
#include <toolbox/sprite-sheet.h>
#include <toolbox/sprite.h>
typedef enum
{
  NONE,
  UP,
  DOWN,
  LEFT,
  RIGHT
} Direction;

typedef struct Transform
{
  s32 x;
  s32 y;
} Transform;

typedef struct Visual
{
  s32    anchor_x;
  s32    anchor_y;
  Sprite sprite;
} Visual;

typedef struct Animation
{
  s32         current_frame;
  s32         frame_duration;
  SpriteSheet sheet;
} Animation;

typedef struct CommandInput
{
  int desired_dir;
  int action;
} CommandInput;

typedef struct PlayerTag
{
  int dummy;
} PlayerTag;

enum
{
  TRANSFORM,
  VISUAL,
  PLAYER_TAG,
  ANIMATION,
  COMMAND_INPUT,
  NUM_COMPONENTS
};

/**
 *              *ControllerSys*                  *MovenentSys*
 * [PlayerTag] ----------------> [CommandInput] +-----------> [Transform]
 *                                  ^            | *MeleeSys*
 *               *AiSys..*          |            +-----------> [MeleeWeapon]
 * [AiState..]----------------------+
 */

/**
 *
 * ControllerSys[PlayerTag:in, CommandInput: out]
 * AiSys[AiState:in, CommandInput: out]
 * DrawSys[Visual:in, Transform:in]
 * AnimSys[Visual:out, Anim:in]
 * MovenentSys[Transform:in-out, CommandInput: in]
 */


#endif // COMPONENTS_H
