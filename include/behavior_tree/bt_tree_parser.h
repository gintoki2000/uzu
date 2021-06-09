// ngotrung Wed 02 Jun 2021 11:34:23 PM +07
#ifndef BT_TREE_PARSER_H
#define BT_TREE_PARSER_H
#include "SDL_rwops.h"

typedef enum bt_ParserErrCode
{
  BT_PARSER_ERR_NONE,
  BT_PARSER_ERR_INVALID_STRUCTURE,
  BT_PARSER_ERR_INVALID_ELEMENT,
  BT_PARSER_ERR_PARSING,
  BT_PARSER_ERR_IO,
} bt_ParserErrCode;

void bt_parser_init(void);
void bt_parser_shutdown(void);
struct bt_Tree* bt_tree_from_file(const char* path);
struct bt_Tree* bt_tree_from_rwops(SDL_RWops* rwops);
bt_ParserErrCode bt_parser_get_error_code(void);
const char* bt_parser_get_error_string(void);
#endif // BT_TREE_PARSER_H
