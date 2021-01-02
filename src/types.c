#include "types.h"
#include "json_helper.h"

Conversation* conversation_init(Conversation* self)
{
  self->name      = NULL;
  self->responses = ptr_array_new(free);
  self->sentences = ptr_array_new(free);
  return self;
}

void conversation_fini(Conversation* self)
{
  if (self->name != NULL){
    free(self->name);
    self->name = NULL;
  }
  ptr_array_delete(self->responses);
  ptr_array_delete(self->sentences);
  self->responses = NULL;
  self->sentences = NULL;
}
