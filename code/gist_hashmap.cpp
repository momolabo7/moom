#include "momo.h"


// assuming that value is a u32_t and key is c-string
struct gist_hm_entry_t {
  const char* key = 0;
  u32_t value;
};

struct gist_hm_t {
  gist_hm_entry_t entries[256] = {};
  u32_t count = 0;
};

static void
gist_hm_t insert(gist_hm_t* hm, const char* key, u32_t value)
{
  assert(hm->count < array_count(gist_hm_entry_t));
  gist_hm_entry_t entry;
  entry.key = key;
  entry.value = value;

  u32_t hash =  hash_djb2(key);
  if (hm->entries[hash].key) {
    hm->entries[hash] = entry;
  }
  else {
  }

}

int main()
{
}
