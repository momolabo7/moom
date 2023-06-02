// Authors: Gerald Wong 
//
// The idea behind this file is to combine every single thing 
// that needs to be 'debugged' into one giant dbg of information,
// which will be collated and sorted out at the very end.
//
// There are a bunch of things you can push into the dbg, but they
// generally fall into 3 main categories:
// - arena information 
// - CPU cycle snapshots/count/hits
// - variable snapshot/inspection
//

#ifndef _MOMO_DBG_
#define _MOMO_DBG_

#define DBG_MAX_EVENTS 256

struct dbg_timer_snapshot_t {
  u32_t hits;
  u32_t cycles;
};

struct dbg_timer_event_t {
  u32_t line;
  const char* name;
  const char* filename;
  u64_t hits_and_cycles;

};

enum dbg_event_type_t {
  DBG_EVENT_TYPE_INSPECT_F32,
  DBG_EVENT_TYPE_INSPECT_U32,

  DBG_EVENT_TYPE_BEGIN_TIMER,
  DBG_EVENT_TYPE_END_TIMER,
};

struct dbg_event_begin_timer_t {

  u32_t start_cycles;
  u32_t start_hits;
};

struct dbg_event_end_timer_t {
  u32_t end_cycles;
  u32_t end_hits;
};

struct dbg_event_t {
  dbg_event_type_t type;
  union {
    u32_t inspect_u32;
    f32_t inspect_f32;
  };
};

struct dbg_t {
  dbg_event_t events[DBG_MAX_EVENTS];
  u32_t event_count;
};

static void dbg_inspect_f32(dbg_t* dbg, f32_t value) {
  assert(dbg->event_count < array_count(dbg->entries));
  dbg_event_t* event = dbg->entries + dbg->event_count++;
  event->type = DBG_EVENT_TYPE_INSPECT_F32;
  event->inspect_f32 = value;
}

static void dbg_inspect_u32(dbg_t* dbg, f32_t value) {
  assert(dbg->event_count < array_count(dbg->entries));
  dbg_event_t* event = dbg->entries + dbg->event_count++;
  event->type = DBG_EVENT_TYPE_INSPECT_U32;
  event->inspect_u32 = value;
}


//
// Profiler API
//
static dbg_profile_event_t*
_dbg_init_profile_event(dbg_t dbg) {

  if (p->event_count < p->event_cap) {
    profiler_event_t* event = p->entries + p->event_count++;
    event->filename = filename;
    event->block_name = block_name ? block_name : function_name;
    event->line = line;
    event->start_cycles = (u32_t)p->get_performance_counter();
    event->start_hits = 1;
    event->flag_for_reset = false;
    return event;
  }

  return nullptr;
}

static void 
_dbg_begin_profile(dbg_t* dbg, usz_t event_index) 
{
  assert(dbg->event_count < array_count(dbg->entries));
  dbg_event_t* event = dbg->entries + dbg->event_count++;
  event->type = DBG_EVENT_TYPE_BEGIN_PROFILE; 
  event->
}


static void 
_dbg_end_profile(dbg_t* dbg, usz_t event_index) 
{
  assert(dbg->event_count < array_count(dbg->entries));
  dbg_event_t* event = dbg->entries + dbg->event_count++;
  event->type = DBG_EVENT_TYPE_BEGIN_PROFILE; 
}




#endif //_MOMO_DBG_
