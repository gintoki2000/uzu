#include "struct_meta_data.h"
#include "global.h"

static FieldMetaData _fields_Conversation[] = {
  JSON_FIELD(Conversation, JSON_FIELD_TYPE_STRING, name),
  JSON_FIELD(Conversation, JSON_FIELD_TYPE_STRING_ARRAY, sentences),
  JSON_FIELD(Conversation, JSON_FIELD_TYPE_STRING_ARRAY, responses),
  JSON_FIELD_END,
};

const StructMetaData gStructMetaData_Conversation = JSON_META_DATA(Conversation, _fields_Conversation);
