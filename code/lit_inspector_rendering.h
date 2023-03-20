/* date = June 16th 2022 6:54 pm */

#ifndef LIT_INSPECTOR_RENDERING_H
#define LIT_INSPECTOR_RENDERING_H


static void 
inspector_update_and_render(lit_t* lit) 
{
  inspector_t* inspector = &lit->inspector;
  gfx_t* gfx = lit->gfx;
  assets_t* assets = &lit->assets;
  gfx_push_asset_sprite(gfx, assets, lit->blank_sprite, 
                          v2f_set(LIT_WIDTH/2, LIT_HEIGHT/2), 
                          v2f_set(LIT_WIDTH, LIT_HEIGHT),
                          rgba_set(0.f, 0.f, 0.f, 0.5f));
  gfx_advance_depth(gfx);
  
  f32_t line_height = 32.f;
  sb8_make(sb, 256);
  
  for(u32_t entry_index = 0; entry_index < inspector->entry_count; ++entry_index)
  {
    inspector_entry_t* entry = inspector->entries + entry_index;
    switch(entry->type){
      case INSPECTOR_ENTRY_TYPE_U32: {
        u32_t item = *(u32_t*)entry->item;
        sb8_push_fmt(sb, str8_from_lit("[%10S] %7u"),
                     entry->name, item);
      } break;
      case INSPECTOR_ENTRY_TYPE_F32: {
        f32_t item = *(f32_t*)entry->item;
        sb8_push_fmt(sb, str8_from_lit("[%10S] %7f"),
                     entry->name, item);
      } break;
    }
    
    
    
    f32_t y = LIT_HEIGHT - line_height * (entry_index+1);
    
    gfx_push_text(gfx, assets, lit->debug_font, sb->str, rgba_hex(0xFFFFFFFF), 0.f, y, line_height);
    gfx_advance_depth(gfx);
    
    
  }
}

#endif //LIT_INSPECTOR_RENDERING_H
