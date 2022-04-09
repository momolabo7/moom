#include "momo.h"
#include "game.h"


#include "game_profiler.h"


static B32
game_init(Game_Memory* memory) {
  
  // Initialization
  if (!memory->state) {
    
    
    game_log("initialized!");
    memory->state = (Game_State*)platform.alloc(sizeof(Game_State));
    if (!memory->state) return false;
    
    platform.set_aspect_ratio(16, 9);
    
    
    // Init arenas
    {
      init_arena(&memory->state->asset_arena, platform.alloc(MB(20)), MB(20));
      init_arena(&memory->state->debug_arena, platform.alloc(MB(1)), MB(1));
      init_arena(&memory->state->frame_arena, platform.alloc(MB(1)), MB(1));
    }
    
    B32 success = load_game_assets(&memory->state->game_assets, 
                                   memory->texture_queue,
                                   "test.sui",
                                   &memory->state->asset_arena);
    if(!success) return false;
    
    
    // Initialize perm memory
    Sandbox_Mode* sandbox = &memory->state->sandbox_mode;
    sandbox->tmp_delta = 0.f;
    sandbox->tmp_increase = true;
    sandbox->tmp_rot = 0.f;
    
    // Initialize Debug Console
    Console* dc = &memory->state->console;
    init_console(dc, &memory->state->debug_arena);
    
    // Initialize profiler 
    // TODO(Momo): Profiler should really be a seperate system
    // on it's own. Maybe it's own module even!
    init_profiler(1234, 32, &memory->state->debug_arena);
    
  }
  
  return true;
  
}

exported B32 
game_update(Game_Memory* memory,
            Game_Input* input, 
            Game_Render_Commands* render_commands) 
{ 
  platform = memory->platform_api;
  
  F32 dt = input->seconds_since_last_frame;
  
  if (!game_init(memory)) {
    return false;
  }
  
  // Actual update here.
  Sandbox_Mode* sandbox = &memory->state->sandbox_mode;
  Console* dc = &memory->state->console;
  Game_Assets* ga = &memory->state->game_assets;
  
  
  update_console(dc, input);
  
  
  // Clear colors
  {
    RGBA colors;
    colors.r = colors.g = colors.b  = colors.a = 0.3f;
    push_colors(render_commands, colors);
  }
  
  // Set camera
  {
    V3 position = {};
    Rect3 frustum;
    frustum.min.x = frustum.min.y = frustum.min.z = 0;
    frustum.max.x = 1600;
    frustum.max.y = 900;
    frustum.max.z = 500;
    push_orthographic_camera(render_commands, position, frustum);
  }
  
  {
    if (sandbox->tmp_increase)
      sandbox->tmp_delta += dt; 
    else
      sandbox->tmp_delta -= dt;
    
    if (sandbox->tmp_delta >= 1.f ){
      sandbox->tmp_delta = 1.f;
      sandbox->tmp_increase = false;
    }
    
    if (sandbox->tmp_delta <= 0.f) {
      sandbox->tmp_delta = 0.f;
      sandbox->tmp_increase = true;
    }
    
    RGBA colors = create_rgba(1.f, 1.f, 1.f, 1.f);
    M44 s = create_m44_scale(600.f, 600.f, 10.f);
    M44 r = create_m44_rotation_z(sandbox->tmp_rot += dt);
    M44 t = create_m44_translation(800.f, 450.f, 300.f);
    
    {
#if 1
      Sprite_Asset* sprite = get_sprite(ga, SPRITE_BULLET_CIRCLE);
      assert(sprite);
      Bitmap_Asset* bitmap = get_bitmap(ga, sprite->bitmap_id);
      assert(bitmap);
      
      push_subsprite(render_commands, 
                     colors,
                     t*r*s,
                     bitmap->renderer_texture_handle, 
                     sprite->uv);
#else
      Font_Asset* font = get_font(ga, FONT_DEFAULT);
      assert(font);
      
      Font_Glyph_Asset* glyph = get_glyph(font, 65);
      
      push_subsprite(render_commands, 
                     colors,
                     t*r*s,
                     0, 
                     glyph->uv);
#endif
    }
  }
  
  render_console(dc, ga, render_commands);
  return true;
  
}