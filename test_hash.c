#include "toolbox/hash_table.h"

static u32 hash_string(const char* str)
{
  u32 hash = 5381;
  int c;

  while ((c = *(str++)))
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

  return hash;
}

static BOOL equal_string(const char* lhs, const char* rhs)
{
  return !SDL_strcmp(lhs, rhs);
}

static void to_string(void* k, void* v, char* buff)
{
  sprintf(buff, "(%s, %s)", k, v);
}

int main()
{
  HashTable* ht = hash_table_create((HashFunc)hash_string, (EqualFunc)equal_string, NULL);

  struct item
  {
    char *k, *v;
  } items[] = { { "foo", "bar" },
                { "trung", "ngo" },
                { "long", "nhat" },
                { "bla", "bla" },
                { "a short string", "bla" },
                { "some more string ^^", "ho ho ho" },
                { "message", "c is amazing!!" },
                { "message 2", "i can do any thing with c" },
                { "message", "c is amazing modified" },
                { "hdhdhd", "adada" },
                { "jhdhye", "eyhdhj" },
                { "dark", "lmao" },
                { "buruh", "lamao" },
                { "lamao", "lick my ass onichan^^" } };
  u32 n     = sizeof(items) / sizeof(struct item);
  for (u32 i = 0; i < n; ++i)
    hash_table_insert(ht, items[i].k, items[i].v);

  hash_table_print(ht, to_string);

  char* k = "a short string";
  char* v = hash_table_lookup(ht, k);
  printf("%s->%s\n", k, v);

  ASSERT(hash_table_count(ht) == n);

  hash_table_free(ht);

  return 0;
}
