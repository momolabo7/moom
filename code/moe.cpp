#include "moe.h"

/////////////////////////////////////////////////////////
// GAME
// 

exported B32 
moe_update_and_render(Platform* pf)
{ 
  if (pf->reloaded) {
    if(pf->moe) {
      pf->free_memory((Platform_Memory_Block*)pf->moe);
    }
   
    // Allocate and initialize Moe engine
    Platform_Memory_Block* moe_memory = pf->allocate_memory(megabytes(32));
    pf->moe = moe_memory;
    
    Moe* moe = (Moe*)moe_memory->data;
    moe->platform = pf;
    
    // TODO: we should make a function out of this
    arn_init(&moe->main_arena, (U8*)moe_memory->data + sizeof(Moe), moe_memory->size - sizeof(Moe)); 
    //platform->moe = arn_push(Moe, &moe_main_arena);

    // around 32MB worth
    if (!arn_partition(&moe->main_arena, &moe->asset_arena, megabytes(20), 16)) 
      return false;
    if (!arn_partition(&moe->main_arena, &moe->scene_arena, megabytes(5), 16)) 
      return false; 
    if (!arn_partition(&moe->main_arena, &moe->debug_arena, megabytes(1), 16)) 
      return false;
    if (!arn_partition(&moe->main_arena, &moe->frame_arena, megabytes(1), 16)) 
      return false;
   
    if(!moe_init_assets(moe, "test_pack.sui"))
      return false;
   
    moe->blank_sprite = find_first_sprite(&moe->assets, ASSET_GROUP_TYPE_BLANK_SPRITE);
    // Debug font
    {
      make(Asset_Match, match);
      set_match_entry(match, ASSET_TAG_TYPE_FONT, 1.f, 1.f);
      moe->debug_font = find_best_font(&moe->assets, ASSET_GROUP_TYPE_FONTS, match);
    }

    moe_goto_scene(moe, moe_entry_scene);
    
    // Initialize Debug Console
    Console* console = &moe->console;
    init_console(console, &moe->debug_arena);
    
    moe->show_debug_type = MOE_SHOW_DEBUG_NONE;
    moe->is_done = false;
    
    // Inform platform what our moe's dimensions are
    pf->set_moe_dims(MOE_WIDTH, MOE_HEIGHT);

    // set up view for moe
    gfx_push_view(pf->gfx, 0.f, MOE_WIDTH, 0.f, MOE_HEIGHT, 0.f, 0.f);

    moe_log("Initialized!");
   
  }
 
  Moe* moe = (Moe*)((Platform_Memory_Block*)pf->moe)->data;
  moe_profile_block(GAME);
  Console* console = &moe->console;

 
#if 0
  // TODO: should probably be in scenes instead
  insp_clear(inspector);
  static U32 test_value = 32;
  insp_add_u32(in, str8_from_lit("Test"), &test_value);
#endif

  
  // Moe state management
  if (moe->is_scene_changed) {
    arn_clear(&moe->scene_arena);
    moe->scene_context = null;
    moe->is_scene_changed = false;
  }
  moe->scene_tick(moe);

  // Debug Rendering Stuff
  if (pf_is_button_poked(pf->button_console)) {
    moe->show_debug_type = 
      (Moe_Show_Debug_Type)((moe->show_debug_type + 1)%MOE_SHOW_DEBUG_MAX);
  }

  moe_profile_begin(DEBUG);
  switch (moe->show_debug_type) {
    case MOE_SHOW_DEBUG_CONSOLE: {
      update_and_render_console(moe); 
    }break;
    case MOE_SHOW_DEBUG_PROFILER: {
      update_and_render_profiler(moe); 
    }break;
    case MOE_SHOW_DEBUG_INSPECTOR: {
      update_and_render_inspector(moe);
    }break;
    default: {}
  }
  moe_profile_end(DEBUG);

#if 0
  static F32 sine = 0.f;
  Platform_Audio* audio = platform->audio;
  S16* sample_out = audio->sample_buffer;
  S16 volume = 3000;
  for(U32 sample_index = 0; sample_index < audio->sample_count; ++sample_index) {
    for (U32 channel_index = 0; channel_index < audio->channels; ++channel_index) {
      F32 sine_value = sin(sine);
      sample_out[channel_index] = S16(sine_value * volume);
    }
    sample_out += audio->channels;
    sine += 2.0f;
  }
#endif

  return moe->is_done;
  
}
