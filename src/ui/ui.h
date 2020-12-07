#ifndef UI_H
#define UI_H
#include <toolbox/toolbox.h>

typedef struct _UI     UI;
typedef struct _UIVtbl UIVtbl;

struct _UI
{
  const UIVtbl* vtbl;
  BOOL          visible;
  SDL_Point     pos;
};

struct _UIVtbl
{
  void (*fini)(UI*);
  void (*on_paint)(UI*, SDL_Renderer*);
};

#define UI(__obj) ((UI*)__obj)
#define UI_VTBL(__vtbl) ((UIVtbl*)__vtbl)

#endif //__UINT16_C_SUFFIX___H
