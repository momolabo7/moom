#include "moe.h"

exported b32_t 
moe_update_and_render(platform_t* pf)
{ 
  if (pf->moe_data == nullptr) {
  
    // Allocate and initialize moe_t engine
    umi_t moe_memory_size = megabytes(32); 
    platform_memory_t* memory = pf->allocate_memory(moe_memory_size);
    pf->moe_data = memory;

    // This is how we are going to get the data
    moe_t* moe = (moe_t*)((platform_memory_t*)pf->moe_data)->data;
    
    moe->platform = pf;
    
    // TOOD(momo): We should totally ask platform to do a free list...? of sorts?
    // Or some kind of expandable arena. Then I don't have to do this strange memory-foo!
    arena_init(&moe->main_arena, (u8_t*)moe + sizeof(moe_t), moe_memory_size - sizeof(moe_t)); 

    // around 32MB worth
    if (!arena_partition(&moe->main_arena, &moe->asset_arena, megabytes(20), 16)) 
      return false;
    if (!arena_partition(&moe->main_arena, &moe->debug_arena, megabytes(1), 16)) 
      return false;
    if (!arena_partition(&moe->main_arena, &moe->frame_arena, megabytes(1), 16)) 
      return false;
   
    //if(!moe_init_assets(moe, "test_pack.sui"))
    //  return false;
   

    // Initialize Debug Console
    console_t* console = &moe->console;
    console_init(console, 256, &moe->debug_arena);
    
    moe->is_done = false;
    
    // Inform platform what our moe's dimensions are
    pf->set_moe_dims(MOE_WIDTH, MOE_HEIGHT);

    // set up view for moe
    gfx_push_view(pf->gfx, 0.f, MOE_WIDTH, 0.f, MOE_HEIGHT, 0.f, 0.f);

    moe_log("Initialized!");
   
  }
 
  moe_t* moe = (moe_t*)((platform_memory_t*)pf->moe_data)->data;
  moe_profile_block(GAME);
  console_t* console = &moe->console;

 
  //game_tick(moe);
  lit_tick_v2(moe);

  // Debug Rendering Stuff
  

  //moe_profile_begin(DEBUG);
  //moe_profile_end(DEBUG);

#if 0 
  static f32_t sine = 0.f;
  platform_audio_t* audio = pf->audio;
  s16_t* sample_out = audio->sample_buffer;
  s16_t volume = 3000;
  for(u32_t sample_index = 0; sample_index < audio->sample_count; ++sample_index) {
    for (u32_t channel_index = 0; channel_index < audio->channels; ++channel_index) {
      f32_t sine_value = f32_sin(sine);
      sample_out[channel_index] = s16_t(sine_value * volume);
    }
    sample_out += audio->channels;
    sine += 0.05f;
    if (sine >= PI_32) sine = 0.f;
  }
#endif

  return moe->is_done;
  
}
