#include "moe.h"

/////////////////////////////////////////////////////////
// GAME
// 

exported B32 
moe_update_and_render(Platform* pf)
{ 
  // Set globals from platform
  platform = pf;
  gfx = platform->gfx;
#if INTERNAL
  profiler = platform->profiler;
#endif


  moe_profile_block(GAME);
  // Initialization
  if (!platform->moe || platform->reloaded) {
    arn_clear(platform->moe_arena);
    platform->moe = arn_push(Game, platform->moe_arena);
    Game* moe = (Game*)platform->moe;

    // around 32MB worth
    if (!arn_partition(platform->moe_arena, &moe->asset_arena, MB(20), 16)) 
      return false;
    if (!arn_partition(platform->moe_arena, &moe->mode_arena, MB(5), 16)) 
      return false; 
    if (!arn_partition(platform->moe_arena, &moe->debug_arena, MB(1), 16)) 
      return false;
    if (!arn_partition(platform->moe_arena, &moe->frame_arena, MB(1), 16)) 
      return false;
    
    if(!init_moe_assets(&moe->assets, 
                         "test_pack.sui",
                         &moe->asset_arena))
    {
      return false;
    }
   
    moe->blank_sprite = find_first_sprite(&moe->assets, asset_group(BLANK_SPRITE));
    // Debug font
    {
      make(Moe_Asset_Match, match);
      set_match_entry(match, asset_tag(FONT), 1.f, 1.f);
      moe->debug_font = find_best_font(&moe->assets, asset_group(FONTS), match);
    }

    moe_goto_mode(moe, MOE_MODE_TYPE_LIT);

    
    //moe_set_mode(moe, splash_init, splash_tick);
    //moe_set_mode(moe, lit_init, lit_tick);
    
    // Initialize Debug Console
    Console* console = &moe->console;
    init_console(console, &moe->debug_arena);
    
    moe->show_debug_type = MOE_SHOW_DEBUG_NONE;
    moe->is_done = false;
    
    // Inform platform what our moe's dimensions are
    platform->set_moe_dims(MOE_WIDTH, MOE_HEIGHT);

    // set up view for moe
    gfx_push_view(gfx, 0.f, MOE_WIDTH, 0.f, MOE_HEIGHT, 0.f, 0.f);

    moe_log("Initialized!");
   
  }
 
  // Set globals from moe
  Game* moe = (Game*)platform->moe;
  assets = &moe->assets;
  inspector = &moe->inspector;
  Console* console = &moe->console;

 
#if 0
  // TODO: should probably be in modes instead
  insp_clear(inspector);
  static U32 test_value = 32;
  insp_add_u32(in, str8_from_lit("Test"), &test_value);
#endif

  
  // Game state management
  if (moe->is_mode_changed) {
    arn_clear(&moe->mode_arena);
    moe->mode_context = null;
    moe->is_mode_changed = false;
  }
  moe_modes[moe->current_moe_mode](moe);

  // Debug Rendering Stuff
  if (pf_is_button_poked(platform->button_console)) {
    moe->show_debug_type = 
      (Moe_Show_Debug_Type)((moe->show_debug_type + 1)%MOE_SHOW_DEBUG_MAX);
  }

  moe_profile_begin(DEBUG);
  switch (moe->show_debug_type) {
    case MOE_SHOW_DEBUG_CONSOLE: {
      update_and_render_console(console,
                                moe->blank_sprite, 
                                moe->debug_font); 
    }break;
    case MOE_SHOW_DEBUG_PROFILER: {
      update_and_render_profiler(moe->blank_sprite, 
                                 moe->debug_font); 
    }break;
    case MOE_SHOW_DEBUG_INSPECTOR: {
      update_and_render_inspector(moe->blank_sprite, 
                                  moe->debug_font);
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
