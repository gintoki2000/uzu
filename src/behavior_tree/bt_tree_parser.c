#include "behavior_tree/bt_tree_parser.h"
#include "behavior_tree/bt_composite_node.h"
#include "behavior_tree/bt_decorator_node.h"
#include "behavior_tree/bt_node.h"
#include "behavior_tree/bt_root.h"
#include "behavior_tree/bt_selector.h"
#include "behavior_tree/bt_sequence.h"
#include "behavior_tree/bt_tree.h"
#include "behavior_tree/bt_wait_task.h"
#include "behavior_tree/bt_get_random_location_task.h"
#include "behavior_tree/bt_walk_directly_toward_task.h"
#include "expat.h"
#include "toolbox/hash_table.h"

#define MAX_DEPTH 32

STATIC_ASSERT(sizeof(char) == sizeof(XML_Char), xml_char_size_should_equal_to_char_size);


typedef void* (*_StartFunc)(XML_Parser parser, const XML_Char** attrs);
typedef void (*_EndFunc)(XML_Parser parser);

typedef struct
{
  _StartFunc startFunc;
  _EndFunc   endFunc;
} _ElementHandler;

typedef struct
{
  void* data;
  char  element[128];
} _StackItem;

static _ElementHandler* element_handler_create(_StartFunc startFunc, _EndFunc endFunc)
{
  _ElementHandler* h = SDL_malloc(sizeof(_ElementHandler));
  h->endFunc         = endFunc;
  h->startFunc       = startFunc;
  return h;
}

static void element_handler_free(_ElementHandler* h)
{
  SDL_free(h);
}

static void set_error_string(const char* errorString);
#define REPORT_ERR(errCode, errorString)                                       \
  do {                                                                         \
    _errorCode = errCode;                                                      \
    set_error_string(errorString);                                             \
    return;                                                                    \
  } while (0)
#define REPORT_ERR_EX(errCode, errorString, returnValue)                       \
  do {                                                                         \
    _errorCode = errCode;                                                      \
    set_error_string(errorString);                                             \
    return (returnValue);                                                      \
  } while (0)
#define CLEAR_ERROR()                                                          \
  do {                                                                         \
    _errorCode = BT_PARSER_ERR_NONE;                                           \
    set_error_string("");                                                      \
  } while (0)
static const char* xml_find_attribute(const char** attrs, const char* attr);
static BOOL xml_get_attribute_as_float(const char** attributes,
                                       const char*  attributeName,
                                       float*       outAttributeValue);
static void        init_stack_item(_StackItem* stackItem, void* data, const char* element);

static u32 hash_string(const char* str)
{
  u32 hash = 5381;
  int c;

  while ((c = *str++))
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

  return hash;
}

static BOOL equal_string(const char* lhs, const char* rhs)
{
  return !SDL_strcmp(lhs, rhs);
}

/* handler callback functions */
static void dummy_end_element_callback(SDL_UNUSED XML_Parser parser)
{
}

static void* dummy_start_element_callback(SDL_UNUSED XML_Parser parser,
                                          SDL_UNUSED const XML_Char** attrs)
{
  return NULL;
}

static void  end_blackboard_keys_element(XML_Parser parser);
static void  end_root_element(XML_Parser parser);
static void* start_blackboard_key_element(XML_Parser parser, const XML_Char** attrs);
static void* start_behavior_tree_element(XML_Parser parser, const XML_Char** attrs);
static void* start_blackboard_keys_element(XML_Parser parser, const XML_Char** attrs);
static void* start_root_element(XML_Parser parser, const XML_Char** attrs);
static void* start_selector_element(XML_Parser parser, const XML_Char** attrs);
static void* start_sequence_element(XML_Parser parser, const XML_Char** attrs);
static void* start_wait_element(XML_Parser parser, const XML_Char** attrs);
static void* start_walk_directly_torward_element(XML_Parser parser, const XML_Char** attrs);
static void* start_get_random_location_element(XML_Parser parser, const XML_Char** attrs);

static void end_node_element(XML_Parser parser);

static void XMLCALL start_element(XML_Parser parser, const char* element, const XML_Char** attrs);

static void XMLCALL end_element(XML_Parser parser, const char* element);

HashTable*            _handlerTable;
_StackItem            _stack[MAX_DEPTH];
u32                   _depth;
enum bt_ParserErrCode _errorCode;
char                  _errorString[256];

#define STACK_TOP (_depth - 1)
#define STACK_TOP_ITEM (_stack[STACK_TOP])

static const _ElementHandler* get_element_handler(const char* element);
static void add_element_handler(char* element, _StartFunc startFunc, _EndFunc endFunc);

void bt_xml_parser_init(void)
{
  _handlerTable = hash_table_create((HashFunc)hash_string,
                                    (EqualFunc)equal_string,
                                    (DestroyFunc)element_handler_free);
  add_element_handler("BehaviorTree", start_behavior_tree_element, dummy_end_element_callback);
  add_element_handler("Root", start_root_element, end_root_element);
  add_element_handler("Sequence", start_sequence_element, end_node_element);
  add_element_handler("Selector", start_selector_element, end_node_element);
  add_element_handler("Wait", start_wait_element, end_node_element);
  add_element_handler("BlackboardKeys", start_blackboard_keys_element, end_blackboard_keys_element);
  add_element_handler("BlackboardKey", start_blackboard_key_element, dummy_end_element_callback);
  add_element_handler("WalkDirectlyToward", start_walk_directly_torward_element, end_node_element);
  add_element_handler("GetRandomLocaltion", start_get_random_location_element, end_node_element);
}

void bt_xml_parser_shutdown(void)
{
  hash_table_free_null(&_handlerTable);
}

#define BUF_SIZE 8192

bt_Tree* bt_tree_from_rwops(SDL_RWops* rwops)
{

  XML_Parser      parser;
  enum XML_Status status;
  char*           buf;
  size_t          byteRead;
  bt_Tree*        tree = NULL;

  CLEAR_ERROR();
  parser = XML_ParserCreate(NULL);
  XML_SetElementHandler(parser,
                        (XML_StartElementHandler)start_element,
                        (XML_EndElementHandler)end_element);

  for (;;)
  {
    buf      = XML_GetBuffer(parser, BUF_SIZE);
    byteRead = SDL_RWread(rwops, buf, 1, BUF_SIZE);

    status = XML_ParseBuffer(parser, byteRead, byteRead == 0);
    if (status == XML_STATUS_ERROR)
    {
      _errorCode = BT_PARSER_ERR_PARSING;
      set_error_string(XML_ErrorString(XML_GetErrorCode(parser)));
      break;
    }
  }
  XML_ParserFree(parser);
  if (status == XML_STATUS_OK)
  {
    if (_depth != 1)
    {
      _errorCode = BT_PARSER_ERR_INVALID_STRUCTURE;
      set_error_string("Invalid structure");
    }
    else
    {
      tree = _stack[0].data;
    }
  }
  return tree;
}

bt_Tree* bt_tree_from_file(const char* file)
{
  SDL_RWops* rwops;
  bt_Tree*   tree = NULL;

  if ((rwops = SDL_RWFromFile(file, "rw")) != NULL)
  {
    tree = bt_tree_from_rwops(rwops);
    SDL_RWclose(rwops);
  }
  return tree;
}

static void add_element_handler(char* element, _StartFunc startFunc, _EndFunc endFunc)
{
  hash_table_insert(_handlerTable, element, element_handler_create(startFunc, endFunc));
}

static const _ElementHandler* get_element_handler(const char* element)
{
  return hash_table_lookup(_handlerTable, (void*)element);
}

static void XMLCALL start_element(XML_Parser parser, const char* element, const XML_Char** attrs)
{
  const _ElementHandler* elementHandler;

  if (_depth == 0 && SDL_strcmp(element, "BehaviorTree"))
  {
    XML_StopParser(parser, XML_FALSE);
    REPORT_ERR(BT_PARSER_ERR_INVALID_STRUCTURE, "Depth level 0 must be \"BehaviorTree\" element");
  }

  elementHandler = get_element_handler(element);

  if (elementHandler)
    init_stack_item(&_stack[_depth++], elementHandler->startFunc(parser, attrs), element);
  else
  {
    XML_StopParser(parser, XML_FALSE);
    REPORT_ERR(BT_PARSER_ERR_INVALID_ELEMENT, "Invalid Element");
  }
}

static void XMLCALL end_element(XML_Parser parser, const char* element)
{
  const _ElementHandler* elementHandler;

  elementHandler = get_element_handler(element);
  elementHandler->endFunc(parser);

  if (_depth > 1)
    --_depth;
}

// handler function implementations
static void* start_root_element(XML_Parser parser, SDL_UNUSED const XML_Char** attrs)
{
  if (_depth != 1)
  {
    XML_StopParser(parser, XML_FALSE);
    REPORT_ERR_EX(BT_PARSER_ERR_INVALID_STRUCTURE, "Root element must be depth level 1", NULL);
  }
  return bt_root_create();
}

static void end_root_element(SDL_UNUSED XML_Parser parser)
{
  ASSERT(_depth == 2);
  bt_Root* root = _stack[1].data;
  bt_Tree* tree = _stack[0].data;

  bt_tree_set_root(tree, root);
}

static void* start_sequence_element(SDL_UNUSED XML_Parser parser, SDL_UNUSED const XML_Char** attrs)
{
  return bt_sequence_create();
}

static void* start_selector_element(SDL_UNUSED XML_Parser parser, SDL_UNUSED const XML_Char** attrs)
{
  return bt_selector_create();
}

static void* start_wait_element(XML_Parser parser, const XML_Char** attrs)
{
  const char* durationAttr = xml_find_attribute(attrs, "duration");
  if (!durationAttr)
  {
    XML_StopParser(parser, XML_FALSE);
    REPORT_ERR_EX(BT_PARSER_ERR_INVALID_STRUCTURE, "", NULL);
  }
  return bt_wait_task_create((u32)SDL_atoi(durationAttr));
}

static void end_node_element(SDL_UNUSED XML_Parser parser)
{
  bt_Node* parent = _stack[STACK_TOP - 1].data;
  bt_Node* me     = _stack[STACK_TOP].data;
  switch (bt_node_get_type(parent))
  {
  case BT_NODE_TYPE_ROOT:
    if (bt_root_child((bt_Root*)parent) != NULL)
    {
      XML_StopParser(parser, FALSE);
      REPORT_ERR(BT_PARSER_ERR_INVALID_STRUCTURE, "Decorator node can only have one child");
    }
    bt_root_attach((bt_Root*)parent, me);
    break;
  case BT_NODE_TYPE_DECORATOR:
    if (((bt_DecoratorNode*)parent)->child != NULL)
    {
      XML_StopParser(parser, FALSE);
      REPORT_ERR(BT_PARSER_ERR_INVALID_STRUCTURE, "Decorator node can only have one child");
    }
    bt_decorator_node_attach((bt_DecoratorNode*)parent, me);
    break;
  case BT_NODE_TYPE_COMPOSITE:
    bt_composite_node_add((bt_CompositeNode*)parent, me);
    break;
  case BT_NODE_TYPE_TASK:
    REPORT_ERR(BT_PARSER_ERR_INVALID_STRUCTURE, "Task node cannot have child");
  }
}

void* start_blackboard_keys_element(SDL_UNUSED XML_Parser parser, SDL_UNUSED const XML_Char** attrs)
{
  if (_depth != 2)
  {
    REPORT_ERR_EX(BT_PARSER_ERR_INVALID_STRUCTURE,
                  "BlackboardKeys element must be depth level 2",
                  NULL);
  }
  return bt_bbkey_set_create();
}

void end_blackboard_keys_element(SDL_UNUSED XML_Parser parser)
{
  bt_BBKeySet* keySet = _stack[1].data;
  bt_Tree*     tree   = _stack[0].data;
  bt_tree_set_key_set(tree, keySet);
}

void* start_blackboard_key_element(XML_Parser parser, const XML_Char** attrs)
{
  bt_BBKeySet* keySet;
  const char*  name;
  if (_depth != 3 || SDL_strcmp(_stack[STACK_TOP - 1].element, "BlackboardKeys"))
  {
    XML_StopParser(parser, FALSE);
    REPORT_ERR_EX(BT_PARSER_ERR_INVALID_STRUCTURE,
                  "BlackboardKey element must be inside BlackboardKeys element",
                  NULL);
  }
  keySet = _stack[STACK_TOP - 1].data;
  name   = xml_find_attribute(attrs, "name");
  if (!name)
  {
    XML_StopParser(parser, FALSE);
    REPORT_ERR_EX(BT_PARSER_ERR_INVALID_ELEMENT, "BlackboardKey requires name attribute", NULL);
  }

  bt_bbkey_set_add(keySet, name);
  return NULL;
}

static void* start_behavior_tree_element(SDL_UNUSED XML_Parser parser,
                                         SDL_UNUSED const XML_Char** attrs)
{
  return bt_tree_create();
}

static void* start_walk_directly_torward_element(XML_Parser parser, const XML_Char** attrs)
{
  bt_Tree* tree = _stack[0].data;
  bt_BBKeySet* keySet = bt_tree_key_set(tree);
  const char* destinationAttr;
  const char* stopWithinDistanceAttr;
  u32 destination;
  float stopWithinDistance;

  destinationAttr = xml_find_attribute(attrs, "destination");
  stopWithinDistanceAttr = xml_find_attribute(attrs, "stopWithinDistance");
  if (!destinationAttr || !stopWithinDistanceAttr)
  {
      XML_StopParser(parser, XML_FALSE);
      REPORT_ERR_EX(BT_PARSER_ERR_INVALID_ELEMENT, "destination and stopWithinDistance is required!", NULL);
  }

  if (!bt_get_bbkey_index(keySet, destinationAttr, &destination)) {
      XML_StopParser(parser, XML_FALSE);
      REPORT_ERR_EX(BT_PARSER_ERR_INVALID_ELEMENT, "invalid blackboard key", NULL);
  }

  stopWithinDistance = (float)SDL_atof(stopWithinDistanceAttr);

  return bt_walk_directly_toward_task_create(destination, stopWithinDistance);
}

static void* start_get_random_location_element(SDL_UNUSED XML_Parser parser, SDL_UNUSED const XML_Char** attrs)
{
    return NULL;
}
/**********************************************************/

const char* xml_find_attribute(const char** attrs, const char* attrName)
{
  int i;

  for (i = 0; attrs[i]; i += 2)
    if (!SDL_strcmp(attrName, attrs[i]))
      return attrs[i + 1];
  return NULL;
}

static BOOL xml_get_attribute_as_float(const char** attributes,
                                       const char*  attributeName,
                                       float*       outAttributeValue)
{
    const char* attribute = xml_find_attribute(attributes, attributeName);
    if (attribute) {
        *outAttributeValue = (float)SDL_atof(attribute);
        return TRUE;
    }
    return FALSE;
}

static void set_error_string(const char* errorString)
{
  SDL_strlcpy(_errorString, errorString, sizeof(_errorString) - 1);
}

static void init_stack_item(_StackItem* stackItem, void* data, const char* element)
{
  stackItem->data = data;
  SDL_strlcpy(stackItem->element, element, sizeof(stackItem->element) - 1);
}

bt_ParserErrCode bt_parser_get_error_code()
{
   return _errorCode;
}

const char *bt_parser_get_error_string()
{
   return _errorString;
}
