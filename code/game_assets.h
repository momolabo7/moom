#ifndef GAME_ASSETS_H
#define GAME_ASSETS_H


#include "game_asset_file.h"

struct Asset_Bitmap {
  U32 width;
  U32 height;
  U32* pixels;
};

struct Asset_Image {
  Rect2 uv;
  Asset_Bitmap_ID bitmap_id;
};

struct Asset {
  Asset_Type type;
  union {
  	Asset_Bitmap* bitmap;
    Asset_Image * image;
    // etc...
  };
};

struct Game_Assets {
  Arena arena;
  
  Asset assets[ASSET_COUNT];
  
  
  // TODO(Momo): We should remove this
  U32 bitmap_counter;
};

static Game_Assets
create_assets(Platform* pf, Gfx* gfx) {
  Game_Assets ret = {};
  
  UMI memory_size = MB(20);
  void* mem = pf->alloc(memory_size);
  ret.arena = create_arena(mem, memory_size);
  
  
  
  // Read in file
  Platform_File file = pf->open_file("test.sui",
                                     PF_FILE_ACCESS_READ, 
                                     PF_FILE_PATH_EXE);
  assert(!file.error);
  
  // Read header
  SUI_Header sui_header;
  pf->read_file(&file, sizeof(SUI_Header), 0, &sui_header);
  
  // TODO: check magic number
  
  
  for(U32 asset_index = 0; 
      asset_index < ASSET_COUNT;
      ++asset_index) 
  {
    Asset* asset = ret.assets + asset_index;
    
    SUI_Asset_Header ass_header;
    UMI offset_to_current_header = sui_header.offset_to_asset_headers + 
      sizeof(SUI_Asset_Header)*asset_index;
    
    pf->read_file(&file, sizeof(SUI_Asset_Header), 
                  offset_to_current_header, 
                  &ass_header);
    
    
    asset->type = ass_header.type;
    switch(asset->type) {
      case ASSET_TYPE_BITMAP: {
        
        SUI_Bitmap sui_bitmap;
        pf->read_file(&file, sizeof(SUI_Bitmap), 
                      ass_header.offset_to_data, 
                      &sui_bitmap);
        
        U32 bitmap_size = sui_bitmap.width * sui_bitmap.height * 4;
        asset->bitmap = push<Asset_Bitmap>(&ret.arena);
        asset->bitmap->width = sui_bitmap.width;
        asset->bitmap->height = sui_bitmap.height;
        asset->bitmap->pixels = (U32*)push_block(&ret.arena, bitmap_size);
        
        
        pf->read_file(&file, bitmap_size, 
                      ass_header.offset_to_data + sizeof(SUI_Bitmap),
                      asset->bitmap->pixels);
#if 1
        set_texture(gfx, ret.bitmap_counter++, 
                    asset->bitmap->width, 
                    asset->bitmap->height, 
                    asset->bitmap->pixels);
#endif
        
      } break;
      case ASSET_TYPE_IMAGE: {
        SUI_Image sui_image;
        pf->read_file(&file, sizeof(SUI_Image), 
                      ass_header.offset_to_data, 
                      &sui_image);
        
        asset->image = push<Asset_Image>(&ret.arena);
        asset->image->bitmap_id = sui_image.bitmap_id;
        asset->image->uv = sui_image.uv;
      } break;
    }
    
    
    
  }
  
  return ret;
}


static Asset*
get_asset(Game_Assets* ga, U32 asset_index){
  return ga->assets + asset_index;
}

static Asset_Bitmap*
get_bitmap(Game_Assets* ga, Asset_Bitmap_ID bitmap_id) {
  Asset* asset = get_asset(ga, bitmap_id.value);
  assert(asset->type == ASSET_TYPE_BITMAP);
  return asset->bitmap;
}


#endif //GAME_ASSETS_H
