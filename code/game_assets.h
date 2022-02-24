#ifndef GAME_ASSETS_H
#define GAME_ASSETS_H


#include "game_asset_file.h"

struct Asset_Bitmap {
  U32 gfx_bitmap_id; // TODO: tie in with renderer? 
  
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
  //union {
  Asset_Bitmap* bitmap;
  Asset_Image * image;
  // etc...
  //};
};

struct Asset_Group {
  U32 first_asset_id;// TODO: change to Asset_ID
  U32 one_past_last_asset_id;// TODO: change to Asset_ID
};

struct Game_Assets {
  Arena arena;
  
  U32 asset_count;
  Asset* assets;
  
  Asset_Group asset_groups[ASSET_GROUP_COUNT];
  
  
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
  
  // Allocate all possible slots for assets
  ret.assets = push_array<Asset>(&ret.arena, sui_header.asset_count);
  ret.asset_count = sui_header.asset_count;
  
  
  for(U32 asset_group_index = 0; 
      asset_group_index < sui_header.asset_group_count;
      ++asset_group_index) 
  {
    Asset_Group* asset_group = ret.asset_groups + asset_group_index;
    {
      // Look for corresponding SUI_Asset_Group in file
      SUI_Asset_Group sui_asset_group;
      UMI offset_to_sui_asset_group = 
        sui_header.offset_to_asset_groups + sizeof(SUI_Asset_Group)*asset_group_index;
      
      pf->read_file(&file, sizeof(SUI_Asset_Group), 
                    offset_to_sui_asset_group, 
                    &sui_asset_group);
      
      asset_group->first_asset_id = sui_asset_group.first_asset_id;
      asset_group->one_past_last_asset_id = sui_asset_group.one_past_last_asset_id;
    }
    
#if 0
    Asset* test1 = ret.assets + 0;
    test1->bitmap = push<Asset_Bitmap>(&ret.arena);
    
    Asset* test2 = ret.assets + 1;
    test2->image = push<Asset_Image>(&ret.arena);
#endif
    
    // Go through each asset in the group
    for (U32 asset_index = asset_group->first_asset_id;
         asset_index < asset_group->one_past_last_asset_id;
         ++asset_index) 
    {
      Asset* asset = ret.assets + asset_index;
      
      // Look for corresponding SUI_Asset in file
      SUI_Asset sui_asset;
      UMI offset_to_sui_asset = 
        sui_header.offset_to_assets + sizeof(SUI_Asset)*asset_index;
      
      pf->read_file(&file, sizeof(SUI_Asset), 
                    offset_to_sui_asset, 
                    &sui_asset);
      
      // Process the assets
      // NOTE(Momo): For now, we are prefetching EVERYTHING.
      // Might want to not do that in the future?
      asset->type = sui_asset.type;
      switch(asset->type) {
        case ASSET_TYPE_BITMAP: {
          
          SUI_Bitmap sui_bitmap;
          pf->read_file(&file, sizeof(SUI_Bitmap), 
                        sui_asset.offset_to_data, 
                        &sui_bitmap);
          
          U32 bitmap_size = sui_bitmap.width * sui_bitmap.height * 4;
          asset->bitmap = push<Asset_Bitmap>(&ret.arena);
          asset->bitmap->width = sui_bitmap.width;
          asset->bitmap->height = sui_bitmap.height;
          asset->bitmap->pixels = (U32*)push_block(&ret.arena, bitmap_size);
          
          
          pf->read_file(&file, bitmap_size, 
                        sui_asset.offset_to_data + sizeof(SUI_Bitmap),
                        asset->bitmap->pixels);
          
          // send to renderer to manage
          asset->bitmap->gfx_bitmap_id = ret.bitmap_counter++;
          set_texture(gfx, 
                      asset->bitmap->gfx_bitmap_id, 
                      asset->bitmap->width, 
                      asset->bitmap->height, 
                      asset->bitmap->pixels);
          
        } break;
        case ASSET_TYPE_IMAGE: {
          SUI_Image sui_image;
          pf->read_file(&file, sizeof(SUI_Image), 
                        sui_asset.offset_to_data, 
                        &sui_image);
          
          asset->image = push<Asset_Image>(&ret.arena);
          asset->image->bitmap_id = sui_image.bitmap_id;
          asset->image->uv = sui_image.uv;
        } break;
      }
      
      
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

static Asset_Image*
get_image(Game_Assets* ga, Asset_Image_ID image_id) {
  Asset* asset = get_asset(ga, image_id.value);
  assert(asset->type == ASSET_TYPE_IMAGE);
  return asset->image;
}

static U32
get_first_asset(Game_Assets* ga, Asset_Group_ID group_id) {
  Asset_Group* group = ga->asset_groups + group_id;
  if (group->first_asset_id != group->one_past_last_asset_id) {
    return group->first_asset_id;
  }
  return 0;
}

static Asset_Bitmap_ID
get_first_bitmap(Game_Assets* ga, Asset_Group_ID group_id) {
  // TODO: assert? Probably should loop through until we find the first correct asset type
  return {get_first_asset(ga, group_id)};
}


static Asset_Image_ID
get_first_image(Game_Assets* ga, Asset_Group_ID group_id) {
  // TODO: assert? Probably should loop through until we find the first correct asset type
  return {get_first_asset(ga, group_id)};
}



#endif //GAME_ASSETS_H
