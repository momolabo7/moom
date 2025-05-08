#if EDEN_DEBUG


enum eden_debug_element_type_t
{
  EDEN_DEBUG_ELEMENT_TYPE_INSPECT_U32,
};

struct eden_debug_element_t
{
};




enum eden_debug_entry_type_t
{
  // variable inspection
  EDEN_DEBUG_ENTRY_TYPE_INSPECT_U32,
  EDEN_DEBUG_ENTRY_TYPE_INSPECT_F32,
  EDEN_DEBUG_ENTRY_TYPE_INSPECT_S32,
  EDEN_DEBUG_ENTRY_TYPE_INSPECT_V2F,
  EDEN_DEBUG_ENTRY_TYPE_INSPECT_ARENA,
};

struct eden_debug_entry_t
{
  buf_t name;
  eden_debug_entry_type_t type;
  union {
    u32_t inspect_u32;
    f32_t inspect_f32;
    s32_t inspect_s32;
    v2f_t inspect_v2f;
    arena_t inspect_arena;
  };
};

struct eden_debug_t 
{
  arena_t* arena; 

  u32_t entry_count;
  eden_debug_entry_t entries[1024];

  u32_t element_count;
  eden_debug_element_t elements[1024];
};


#endif // EDEN_DEBUG
