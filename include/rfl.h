// ngotrung Thu 10 Jun 2021 09:08:57 PM +07
#ifndef RFL_H
#define RFL_H

typedef enum
{
  RFL_FIELD_TYPE_INT,
  RFL_FIELD_TYPE_STRING,
  RFL_FIELD_TYPE_ENTITY,
  RFL_FIELD_TYPE_STRING_ARRAY,
  RFL_FIELD_TYPE_INT_ARRAY,
  RFL_FIELD_TYPE_OBJECT,
  RFL_NUM_FIELD_TYPES,
};

typedef struct
{
  int                type;
  const char*        name;
  unsigned int       offset;
  struct rfl_Struct* metadata;
} rfl_Field;

#define RFL_FIELD(_T, _type, _name)                                                                \
  {                                                                                                \
    .type = RFL_FIELD_TYPE_##_type, .name = #_name, .offset = offsetof(_T, _name)                  \
  }

#define RFL_FIELD_OBJECT(_T, _name, _metadata)                                                     \
  {                                                                                                \
    .type = RFL_FIELD_TYPE_OBJECT, .name = #_name, .offset = offsetof(_T, _name),                  \
    .metadata = _metadata                                                                          \
  }

#define RFL_FIELD_END                                                                              \
  {                                                                                                \
    .type = -1                                                                                     \
  }

typedef struct rfl_Struct
{
  unsigned int     size;
  const rfl_Field* fields;
} rfl_Struct;

#define RFL_STRUCT(__T, __fields)                                                                  \
  {                                                                                                \
    .size = sizeof(__T), .fields = __fields                                                        \
  }
#endif // RFL_H
