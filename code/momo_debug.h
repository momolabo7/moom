// Authors: Gerald Wong 
//
// The idea behind this file is to combine every single thing 
// that needs to be 'debugged' into one giant pipe of information,
// which will be collated and sorted out at the very end.
//
// There are a bunch of things you can push into the pipe, but they
// generally fall into 3 main categories:
// - arena information 
// - CPU cycle snapshots/count/hits
// - variable snapshot/inspection
//

#ifndef _MOMO_DEBUG_
#define _MOMO_DEBUG_

#define DBG_MAX_ENTRIES 128

struct dbg_cpu_cycle_snapshot_t {
  u32_t hits;
  u32_t cycles;
};

enum dbg_pipe_entry_type_t {
  DEBUG_ENTRY_TYPE_F32,
  DEBUG_ENTRY_TYPE_U32,
};

struct dbg_pipe_entry_t {
  dbg_pipe_entry_type_t type;
  union {
    u32_t u32;
    f32_t f32;
  };
};

struct dbg_pipe_t {
  dbg_pipe_entry_t entries[DBG_MAX_ENTRIES];
  u32_t entry_count;
};

static void dbg_push_f32(dbg_pipe_t* pipe, f32_t value) {
  assert(pipe->entry_count < array_count(pipe->entries));
  dbg_pipe_entry_t* entry = pipe->entries + pipe->entry_count++;
  entry->type = DEBUG_ENTRY_TYPE_F32;
  entry->f32 = value;
}

static void dbg_push_u32(dbg_pipe_t* pipe, f32_t value) {
  assert(pipe->entry_count < array_count(pipe->entries));
  dbg_pipe_entry_t* entry = pipe->entries + pipe->entry_count++;
  entry->type = DEBUG_ENTRY_TYPE_U32;
  entry->u32 = value;
}

#endif //_MOMO_DEBUG_
