#include "SDL.h"
#include "toolbox/common.h"

typedef struct HashTable HashTable;
typedef void (*HashTableForeachCallbackFunc)(void* arg, void* key, void* value);

HashTable* hash_table_create(HashFunc hashFunc, EqualFunc equalFunc, DestroyFunc destroyFunc);
void       hash_table_free(HashTable* ht);
void       hash_table_free_null(HashTable** ht);

void  hash_table_insert(HashTable* self, void* key, void* value);
BOOL  hash_table_remove(HashTable* self, void* key);
void* hash_table_steal(HashTable* self, void* key);
void* hash_table_lookup(HashTable* self, void* key);
u32   hash_table_count(HashTable* self);
u32   hash_table_size(HashTable* self);
void  hash_table_rehash(HashTable* self);
void  hash_table_foreach(HashTable* self, HashTableForeachCallbackFunc callback, void* arg);

void hash_table_print(HashTable* ht, void (*entryToStringFunc)(void* k, void* v, char* buff));
