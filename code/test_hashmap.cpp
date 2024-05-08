#include "momo.h"


static u32_t hashmap[64];

static usz_t
wip_dumbass_hash(u32_t v)
{
  return v % 64;
}

static void
wip_add_item(u32_t key, u32_t value)
{
  u32_t index = wip_dumbass_hash(key);
  hashmap[index] = value;
}


int main() 
{

}
