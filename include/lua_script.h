// ngotrung Mon 07 Jun 2021 10:09:48 PM +07
#ifndef LUA_SCRIPT_H
#define LUA_SCRIPT_H
typedef struct LuaScript LuaScript;

LuaScript* lua_script_create(const char* scriptFile);
void       lua_script_free(LuaScript* script);
#endif // LUA_SCRIPT_H
