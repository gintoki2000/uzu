#ifndef DUNGEON_H
#define DUNGEON_H
#include "global.h"
#include "scene.h"
#include "toolbox/toolbox.h"
void set_scene(const Scene* s);
void push_cursor_state(Cursor cursor);
void pop_cursor_state(void);
#endif // DUNGEON_H
