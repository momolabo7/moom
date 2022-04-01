#include "momo.h"

#include "game.h"

B32 show_console;

Platform_API platform;

exported B32 
game_update(Game_Memory* memory,
            Game_Input* input, 
            Game_Render_Commands* render_commands) { 
  platform = memory->platform_api;
  F32 dt = input->seconds_since_last_frame;
  
  // Initialization
  if (!memory->state) {
    platform.set_aspect_ratio(16, 9);
    
    memory->state = (Game_State*)platform.alloc(sizeof(Game_State));
    if (!memory->state) return false;
    
    // TODO(Momo): Figure out what we want to do with 
    // game asset memories?
    UMI memory_size = MB(20);
    void* mem = platform.alloc(memory_size);
    Arena arena  = create_arena(mem, memory_size);
    B32 success = load_game_assets(&memory->state->game_assets, 
                                   memory->texture_queue,
                                   "test.sui",
                                   &arena);
    if(!success) return false;
    
    
    // Initialize perm memory
    Sandbox_Mode* sandbox = &memory->state->sandbox_mode;
    sandbox->tmp_delta = 0.f;
    sandbox->tmp_increase = true;
    sandbox->tmp_rot = 0.f;
    
  }
  Sandbox_Mode* sandbox = &memory->state->sandbox_mode;
  
  if (is_poked(input->button_up)) {
    platform.hot_reload();
  }
  
  if (is_poked(input->button_console)) {
    show_console = !show_console;
  }
  
  
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
      Game_Assets* ga = &memory->state->game_assets;
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
    
    // Debug console
    if (show_console) {
      Game_Assets* ga = &memory->state->game_assets;
      
      // Camera
      {
        V3 position = {};
        Rect3 frustum;
        frustum.min.x = frustum.min.y = frustum.min.z = 0;
        frustum.max.x = 1600;
        frustum.max.y = 900;
        frustum.max.z = 500;
        push_orthographic_camera(render_commands, position, frustum);
      }
      
      // Draw background
      {
        RGBA bg_color = create_rgba(0.5f, 0.5f, 0.5f, 1.f);
        M44 bgs = create_m44_scale(1600.f, 400.f, 10.f);
        M44 bgt = create_m44_translation(800.f, 200.f, 10.f);
        
        Sprite_Asset* sprite=  ga->sprites + 0;
        push_subsprite(render_commands, 
                       bg_color,
                       bgt*bgs,
                       0, 
                       sprite->uv);
      }
      
      // Draw text
      {
        Font_Asset* font = get_font(ga, FONT_DEFAULT);
        V2 position = {};
        const F32 font_height = 40.f;
        Str8 test_str = str8_from_lit("Hello World");
        for(U32 char_index = 0; 
            char_index < test_str.count;
            ++char_index) 
        {
          U32 curr_cp = test_str.e[char_index];
          if (char_index > 0) {
            U32 prev_cp = test_str.e[char_index-1];
            position.x += get_horizontal_advance(font, prev_cp, curr_cp)*font_height;
          }
          Font_Glyph_Asset *glyph = get_glyph(font, curr_cp);
          
          F32 width = (glyph->box.max.x - glyph->box.min.x)*font_height;
          F32 height = (glyph->box.max.y - glyph->box.min.y)*font_height;
          
          M44 transform = 
            create_m44_translation(position.x + (glyph->box.min.x*font_height), 
                                   position.y + (glyph->box.min.y*font_height), 
                                   9.f)*
            create_m44_scale(width, height, 1.f)*
            create_m44_translation(0.5f, 0.5f, 0.f);
          
          
          
          push_subsprite(render_commands, 
                         colors,
                         transform,
                         0, 
                         glyph->uv);
        }
        
        
      }
    }
    
  }
  
  return true;
  
}