
enum eden_inspector_entry_type_t {
  EDEN_INSPECTOR_ENTRY_TYPE_F32,
  EDEN_INSPECTOR_ENTRY_TYPE_U32,
};

struct eden_inspector_entry_t {
  buf_t name;
  eden_inspector_entry_type_t type;
  union {
    f32_t value_f32;
    u32_t value_u32;
  };
};

struct eden_inspector_t {
  u32_t entry_cap;
  u32_t entry_count;
  eden_inspector_entry_t* entries;
};



