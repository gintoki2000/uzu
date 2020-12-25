#include "components.h"
#include "dungeon.h"
#include "ecs/ecs.h"

#include "system/mediator.h"

#include "system/animator_sys.h"
#include "system/collision_mgr_sys.h"
#include "system/collision_sys.h"
#include "system/drop_system.h"
#include "system/health_sys.h"
#include "system/motion_sys.h"
#include "system/pickup_sys.h"
#include "system/rendering_system.h"

extern void update_rendering_system();

static void on_load();
static void on_unload();
static void on_event(const SDL_Event* evt);
static void on_update();
static void emit_signal(int sig_id, const pointer_t event);
static void load_new_map(const char* filename);

extern EcsType g_comp_types[];

const Screen g_gamescr = {
  .on_load   = on_load,
  .on_unload = on_unload,
  .on_event  = on_event,
  .on_update = on_update,
};

Ecs* g_ecs;

static Dispatcher* _dispatcher;

static void on_load()
{
  mediator_init();
}

static void on_unload()
{
  ecs_del(g_ecs);
  g_ecs = NULL;

  dispatcher_destroy(_dispatcher);
  _dispatcher = NULL;
}

static void on_event(const SDL_Event* evt)
{
  (void)evt;
}

static void on_update()
{
}

void gamescr_connect_sig(int sig_id, slot_t slot, pointer_t arg)
{
  dispatcher_connect(_dispatcher, sig_id, arg, slot);
}
