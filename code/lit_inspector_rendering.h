/* date = June 16th 2022 6:54 pm */

#ifndef LIT_INSPECTOR_RENDERING_H
#define LIT_INSPECTOR_RENDERING_H


static void 
inspector_update_and_render() 
{
  inspector_t* inspector = &lit->inspector;
  assets_t* assets = &lit->assets;
  app_draw_asset_sprite(
      app, 
      assets, 
      lit->blank_sprite, 
      v2f_set(LIT_WIDTH/2, LIT_HEIGHT/2), 
      v2f_set(LIT_WIDTH, LIT_HEIGHT),
      rgba_set(0.f, 0.f, 0.f, 0.5f));
  app_advance_depth(app);

  for(u32_t entry_index = 0; entry_index < inspector->entry_count; ++entry_index)
  {
    f32_t line_height = 32.f;
    stb8_make(sb, 256);


    inspector_entry_t* entry = inspector->entries + entry_index;
    switch(entry->type){
      case INSPECTOR_ENTRY_TYPE_U32: {
        stb8_push_fmt(sb, st8_from_lit("[%10S] %7u"),
            entry->name, entry->item_u32);
      } break;
      case INSPECTOR_ENTRY_TYPE_F32: {
        stb8_push_fmt(sb, st8_from_lit("[%10S] %7f"),
            entry->name, entry->item_f32);
      } break;
    }



    f32_t y = LIT_HEIGHT - line_height * (entry_index+1);

    app_draw_text(app, assets, lit->debug_font, sb->str, rgba_hex(0xFFFFFFFF), 0.f, y, line_height);
    app_advance_depth(app);


  }
}

#endif //LIT_INSPECTOR_RENDERING_H
