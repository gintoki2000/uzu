#include "struct_meta_data.h"
#include "global.h"

static FieldMetaData CONVERSATION_FIELDS[] = {
  JSON_FIELD(Conversation, JSON_FIELD_TYPE_STRING, name),
  JSON_FIELD(Conversation, JSON_FIELD_TYPE_STRING_ARRAY, sentences),
  JSON_FIELD(Conversation, JSON_FIELD_TYPE_STRING_ARRAY, responses),
  JSON_FIELD_END,
};

StructMetaData STRUCT_META_DATA_TBL[NUM_STRUCT_META_DATA_ELEMENTS] = {
  [STRUCT_META_DATA_CONVERSATION] = JSON_META_DATA(Conversation, CONVERSATION_FIELDS),
};
