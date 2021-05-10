enum VariantType
{
  VARIANT_BOOL,
  VARIANT_FLOAT,
  VARIANT_INT,
  VARIANT_STRING,
  VARIANT_VEC2,
};

typedef struct Variant
{
  enum VariantType type;
  union
  {
    int         interger_value;
    double      float_value;
    const char* string_value;
  };
} Variant;

Variant variant_create_as_int(int value)
{ return (Variant){ .type = VARIANT_INT, .interger_value = value }; }

Variant variant_create_as_float(int value)
{ return (Variant){ .type = VARIANT_FLOAT, .float_value = value }; }

Variant variant_create_as_string(const char* value)
{ return (Variant){ .type = VARIANT_STRING, .string_value = value }; } 

int variant_as_integer(const Variant* v)
{ return v->interger_value; }

float variant_as_float(const Variant* v)
{ return v->float_value; }

typedef struct Blackboard
{
  Variant* entries;
} Blackboard;

typedef int BlackboardKey;

int blackboard_get_value_as_int(const Blackboard* blackboard, BlackboardKey key)
{ return variant_as_integer(&blackboard->entries[key]); }
