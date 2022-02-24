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
  union {
    Asset_Bitmap* bitmap;
    Asset_Image*  image;
  };
};

struct Asset_Group {
  Asset_ID first_asset_id;
  Asset_ID one_past_last_asset_id;
};

struct Asset_Tag {
  Asset_Tag_Type type;
  F32 value;
};

struct Game_Assets {
  Arena arena;
  
  U32 asset_count;
  Asset* assets;
  
  U32 tag_count;
  Asset_Tag* tags;
  
  Asset_Group groups[ASSET_GROUP_COUNT];
  
  
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
  Sui_Header sui_header;
  pf->read_file(&file, sizeof(Sui_Header), 0, &sui_header);
  
  // TODO: check magic number
  
  
  // Allocation
  ret.assets = push_array<Asset>(&ret.arena, sui_header.asset_count);
  ret.asset_count = sui_header.asset_count;
  
  ret.tags = push_array<Asset_Tag>(&ret.arena, sui_header.tag_count);
  ret.tag_count = sui_header.tag_count;
  
  // Fill data for tag
  for (U32 tag_index = 0;
       tag_index < ret.tag_count; 
       ++tag_index) 
  {
    Asset_Tag* tag = ret.tags + tag_index;
    
    Sui_Tag sui_tag;
    UMI offset_to_sui_tag = sui_header.offset_to_tags + sizeof(Sui_Tag)*tag_index;
    pf->read_file(&file, sizeof(Sui_Tag), offset_to_sui_tag, &sui_tag);
    
    tag->type = sui_tag.type;
    tag->value = sui_tag.value;
  }
  
  // Fill data for asset groups and individual assets
  for(U32 group_index = 0; 
      group_index < sui_header.group_count;
      ++group_index) 
  {
    Asset_Group* group = ret.groups + group_index;
    {
      // Look for corresponding Sui_Asset_Group in file
      Sui_Asset_Group sui_group;
      UMI offset_to_sui_group = 
        sui_header.offset_to_groups + sizeof(Sui_Asset_Group)*group_index;
      
      pf->read_file(&file, sizeof(Sui_Asset_Group), 
                    offset_to_sui_group, 
                    &sui_group);
      
      group->first_asset_id = sui_group.first_asset_id;
      group->one_past_last_asset_id = sui_group.one_past_last_asset_id;
    }
    
    // Go through each asset in the group
    for (U32 asset_index = group->first_asset_id.value;
         asset_index < group->one_past_last_asset_id.value;
         ++asset_index) 
    {
      Asset* asset = ret.assets + asset_index;
      
      // Look for corresponding Sui_Asset in file
      Sui_Asset sui_asset;
      UMI offset_to_sui_asset = 
        sui_header.offset_to_assets + sizeof(Sui_Asset)*asset_index;
      
      pf->read_file(&file, sizeof(Sui_Asset), 
                    offset_to_sui_asset, 
                    &sui_asset);
      
      // Process the assets
      // NOTE(Momo): For now, we are prefetching EVERYTHING.
      // Might want to not do that in the future?
      asset->type = sui_asset.type;
      switch(asset->type) {
        case ASSET_TYPE_BITMAP: {
          
          Sui_Bitmap sui_bitmap;
          pf->read_file(&file, sizeof(Sui_Bitmap), 
                        sui_asset.offset_to_data, 
                        &sui_bitmap);
          
          U32 bitmap_size = sui_bitmap.width * sui_bitmap.height * 4;
          asset->bitmap = push<Asset_Bitmap>(&ret.arena);
          asset->bitmap->width = sui_bitmap.width;
          asset->bitmap->height = sui_bitmap.height;
          asset->bitmap->pixels = (U32*)push_block(&ret.arena, bitmap_size);
          
          
          pf->read_file(&file, bitmap_size, 
                        sui_asset.offset_to_data + sizeof(Sui_Bitmap),
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
          Sui_Image sui_image;
          pf->read_file(&file, sizeof(Sui_Image), 
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
get_asset(Game_Assets* ga, Asset_ID asset_index){
  return ga->assets + asset_index.value;
}

static Asset_Bitmap*
get_bitmap(Game_Assets* ga, Asset_Bitmap_ID bitmap_id) {
  Asset* asset = get_asset(ga, {bitmap_id.value});
  assert(asset->type == ASSET_TYPE_BITMAP);
  return asset->bitmap;
}

static Asset_Image*
get_image(Game_Assets* ga, Asset_Image_ID image_id) {
  Asset* asset = get_asset(ga, {image_id.value});
  assert(asset->type == ASSET_TYPE_IMAGE);
  return asset->image;
}

static Asset_ID
get_first_asset(Game_Assets* ga, Asset_Group_ID group_id) {
  Asset_Group* group = ga->groups + group_id;
  if (group->first_asset_id.value != group->one_past_last_asset_id.value) {
    return group->first_asset_id;
  }
  return {0};
}

static Asset_Bitmap_ID
get_first_bitmap(Game_Assets* ga, Asset_Group_ID group_id) {
  // TODO: assert? Probably should loop through until we find the first correct asset type
  return {get_first_asset(ga, group_id).value};
}


static Asset_Image_ID
get_first_image(Game_Assets* ga, Asset_Group_ID group_id) {
  // TODO: assert? Probably should loop through until we find the first correct asset type
  return {get_first_asset(ga, group_id).value};
}



#endif //GAME_ASSETS_H
