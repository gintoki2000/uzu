#include "SDL.h"
#include "toolbox/common.h"

typedef struct HashTableEntry
{
  const void*            key;
  void*                  value;
  u32                    hashCode;
  struct HashTableEntry* next;
} HashTableEntry;

typedef struct HashTable
{
  HashFunc    hashFunc;
  EqualFunc   equalFunc;
  DestroyFunc keyDestroyFunc;
  DestroyFunc valDestroyFunc;
  int         count;
  int         capacity;
} HashTable;

HashTable* hash_table_init(HashTable* self, HashFunc hashFunc, EqualFunc equalFunc);
void       hash_table_fini(HashTable* self);

void  hash_table_insert(HashTable* self, const void* key, void* val);
BOOL  hash_table_remove(HashTable* self, const void* key);
void* hash_table_steal(HashTable* self, const void* key);
void* hash_table_lookup(HashTable* self, const void* key);
