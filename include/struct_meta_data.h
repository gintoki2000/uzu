#ifndef STRUCT_META_DATA_H
#define STRUCT_META_DATA_H
#include "json_helper.h"
typedef enum
{
  STRUCT_META_DATA_CONVERSATION,
  NUM_STRUCT_META_DATA_ELEMENTS,
} StructMetaDataId;

extern StructMetaData STRUCT_META_DATA_TBL[];
#endif // STRUCT_META_DATA_H
