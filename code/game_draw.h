/* date = April 7th 2022 10:18 pm */

#ifndef GAME_DRAW_H
#define GAME_DRAW_H


// Draw background
static void
draw_rect(Game_Assets* ga,
          Game_Render_Commands* render_commands,
          RGBA color, 
          F32 px, F32 py, 
          F32 sw, F32 sh, 
          F32 depth)
{
  
  M44 bgs = create_m44_scale(sw, sh, 1.f);
  M44 bgt = create_m44_translation(px, py, depth);
  
  // Blank sprite
  Sprite_Asset* sprite =  ga->sprites + 0;
  push_subsprite(render_commands, 
                 color,
                 bgt*bgs,
                 0, 
                 sprite->uv);
}



#endif //GAME_DRAW_H
