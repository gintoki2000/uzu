#include "struct_meta_data.h"
#include "global.h"

static rfl_Field _conversationFields[] = {
  RFL_FIELD(Conversation, STRING      , name     ),
  RFL_FIELD(Conversation, STRING_ARRAY, sentences),
  RFL_FIELD(Conversation, STRING_ARRAY, responses),
  RFL_FIELD_END,
};

const rfl_Struct gMetaData_Conversation = RFL_STRUCT(Conversation, _conversationFields);
