#ifndef TYPES_H
#define TYPES_H

#include <json-c/json.h>
#include <toolbox/toolbox.h>

typedef struct
{
  char*     name;
  PtrArray* sentences;
  PtrArray* responses;
} Conversation;

Conversation* conversation_init(Conversation* self);
void          conversation_fini(Conversation* self);
#endif // TYPES_H
