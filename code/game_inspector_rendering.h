/* date = June 16th 2022 6:54 pm */

#ifndef GAME_INSPECTOR_RENDERING_H
#define GAME_INSPECTOR_RENDERING_H


static void 
update_and_render_inspector(Inspector* in, Painter* p) 
{
  // TODO
  Game_Sprite_ID sprite_id = get_first_sprite(p->ga, GAME_ASSET_GROUP_TYPE_BLANK_SPRITE);

  paint_sprite(p, 
               sprite_id, 
               GAME_MIDPOINT, 
               GAME_DIMENSIONS,
               {0.f, 0.f, 0.f, 0.5f});
  advance_depth(p);
  
  F32 line_height = 32.f;
  sb8_make(sb, 256);
  
  al_foreach(entry_index, &in->entries)
  {
    Inspector_Entry* entry = al_at(&in->entries, entry_index);
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
    
    Game_Font_ID font_id = get_first_font(p->ga, GAME_ASSET_GROUP_TYPE_DEFAULT_FONT);
    paint_text(p,
               font_id, 
               sb->str,
               hex_to_rgba(0xFFFFFFFF),
               0.f, 
               y, 
               line_height);
    advance_depth(p);
    
    
  }
}


#endif //GAME_INSPECTOR_RENDERING_H
