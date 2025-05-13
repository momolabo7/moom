#if EDEN_DEBUG




//
// @note: 'records' represents an unprocessed recorded event per frame
//
enum eden_debug_record_type_t
{
  // variable inspection
  EDEN_DEBUG_RECORD_TYPE_INSPECT_U32,
  EDEN_DEBUG_RECORD_TYPE_INSPECT_F32,
  EDEN_DEBUG_RECORD_TYPE_INSPECT_S32,
  EDEN_DEBUG_RECORD_TYPE_INSPECT_V2F,
  EDEN_DEBUG_RECORD_TYPE_INSPECT_ARENA,
};

struct eden_debug_record_t
{
  const char* guid;
  const char* name;

  eden_debug_record_type_t type;
  union {
    u32_t inspect_u32;
    f32_t inspect_f32;
    s32_t inspect_s32;
    v2f_t inspect_v2f;
    arena_t inspect_arena;
  };
};

enum eden_debug_element_type_t
{
  EDEN_DEBUG_ELEMENT_TYPE_INSPECT_U32,
};


//
// @note: 
//
// 'element' is quite vague. 
// Theoretically speaking, this represents an actual debug 'thing' after
// processing the debug_records. At the moment, this also represents a 
// view on the screen.
//
struct eden_debug_element_t
{
  // @note: assumed to be a string literal
  const char* guid; 
  eden_debug_element_t* next_in_hash;
  eden_debug_record_t stored_record; //@todo: should be a collection of sorts so that we can collate dataikkk.
};


struct eden_debug_t 
{
  arena_t* arena; 

  u32_t record_count;
  eden_debug_record_t records[1024];

  eden_debug_element_t* hashed_elements[1024];
};


#endif // EDEN_DEBUG
