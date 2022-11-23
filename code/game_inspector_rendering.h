/* date = June 16th 2022 6:54 pm */

#ifndef GAME_INSPECTOR_RENDERING_H
#define GAME_INSPECTOR_RENDERING_H


static void 
update_and_render_inspector(Game_Sprite_ID rect_sprite, Game_Font_ID font) 
{

  paint_sprite(rect_sprite, 
               v2_set(GAME_WIDTH/2, GAME_HEIGHT/2), 
               v2_set(GAME_WIDTH, GAME_HEIGHT),
               {0.f, 0.f, 0.f, 0.5f});
  gfx_advance_depth(gfx);
  
  F32 line_height = 32.f;
  sb8_make(sb, 256);
  
  al_foreach(entry_index, &inspector->entries)
  {
    Inspector_Entry* entry = al_at(&inspector->entries, entry_index);
    switch(entry->type){
      case INSPECTOR_ENTRY_TYPE_U32: {
        U32 item = *(U32*)entry->item;
        sb8_push_fmt(sb, str8_from_lit("[%10S] %7u"),
                     entry->name, item);
      } break;
      case INSPECTOR_ENTRY_TYPE_F32: {
        F32 item = *(F32*)entry->item;
        sb8_push_fmt(sb, str8_from_lit("[%10S] %7f"),
                     entry->name, item);
      } break;
    }
    
    
    
    F32 y = GAME_HEIGHT - line_height * (entry_index+1);
    
    paint_text(font, 
               sb->str,
               hex_to_rgba(0xFFFFFFFF),
               0.f, 
               y, 
               line_height);
    gfx_advance_depth(gfx);
    
    
  }
}


#endif //GAME_INSPECTOR_RENDERING_H
