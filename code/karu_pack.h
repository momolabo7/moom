/* date = February 21st 2022 7:42 pm */

#ifndef KARU_PACK_H
#define KARU_PACK_H


struct Karu_Packer_Bitmap_Source {
  U32 width;
  U32 height;
  U32* pixels;
};

struct Karu_Packer_Image_Source {
  U32 bitmap_asset_id;
  Rect2 uv;
};

struct Karu_Packer_Font_Source {
  Karu_Atlas_Font* ptr;
  U32 bitmap_asset_id;
};

union Karu_Packer_Source {
  Karu_Packer_Bitmap_Source bitmap;
  Karu_Packer_Image_Source image;
};


struct Karu_Packer {
  U32 tag_count;
  Sui_Tag tags[1024]; // to be written to file
  
  U32 asset_count;
  Karu_Packer_Source sources[1024]; // additional data for assets
  Sui_Asset assets[1024]; // to be written to file
  
  Sui_Asset_Group groups[ASSET_GROUP_COUNT]; //to be written to file
  
  // Required context for interface
  Sui_Asset_Group* active_group;
  U32 active_asset_index;
};

static Karu_Packer
begin_sui_packer() {
  Karu_Packer ret = {};
  
  
  ret.asset_count = 1; // reserve for null asset
  ret.tag_count = 1; // reserve to null tag
  
  return ret;
}

static void
add_tag(Karu_Packer* sp, Asset_Tag_Type tag_type, F32 value) {
  U32 tag_index = sp->tag_count++;
  
  Sui_Asset* asset = sp->assets + sp->active_asset_index;
  asset->one_past_last_tag_index = sp->tag_count;
  
  Sui_Tag* tag = sp->tags + tag_index;
  tag->type = tag_type;
  tag->value = value;
  
}

static void
begin_group(Karu_Packer* sp, Asset_Group_ID group_id) 
{
  sp->active_group = sp->groups + group_id;
  sp->active_group->first_asset_index = sp->asset_count;
  sp->active_group->one_past_last_asset_index = sp->active_group->first_asset_index;
}

static void
end_group(Karu_Packer* sp) 
{
  sp->active_group = nullptr;
}

struct Karu_Packer_Added_Asset {
  U32 asset_index;
  Karu_Packer_Source* source;
};

static Karu_Packer_Added_Asset
add_asset(Karu_Packer* sp, Asset_Type type) {
  assert(sp->active_group);
  U32 asset_index = sp->asset_count++;
  ++sp->active_group->one_past_last_asset_index;
  sp->active_asset_index = asset_index;
  
  Sui_Asset* asset = sp->assets + asset_index;
  asset->type = type;
  asset->first_tag_index = sp->tag_count;
  asset->one_past_last_tag_index = asset->one_past_last_tag_index;
  
  Karu_Packer_Source* source = sp->sources + asset_index;
  
  Karu_Packer_Added_Asset ret;
  ret.source = source;
  ret.asset_index = asset_index;
  
  return ret;
}


static U32
add_bitmap(Karu_Packer* sp, Bitmap bitmap) {
  auto added_asset = add_asset(sp, ASSET_TYPE_BITMAP);
  added_asset.source->bitmap.width = bitmap.width;
  added_asset.source->bitmap.height = bitmap.height;
  added_asset.source->bitmap.pixels = bitmap.pixels;
  return added_asset.asset_index;
}


static U32
add_image(Karu_Packer* sp, 
          U32 bitmap_asset_id,
          Rect2 uv)
{
  auto added_asset = add_asset(sp, ASSET_TYPE_IMAGE);
  added_asset.source->image.bitmap_asset_id = bitmap_asset_id;
  added_asset.source->image.uv = uv;
  
  return added_asset.asset_index;
  
}



static void
write_sui(Karu_Packer* sp, const char* filename) {
  FILE* file = fopen(filename, "wb");
  defer { fclose(file); };
  
  U32 asset_tag_array_size = sizeof(Sui_Tag)*sp->tag_count;
  U32 asset_array_size = sizeof(Sui_Asset)*sp->asset_count;
  U32 group_array_size = sizeof(Sui_Asset_Group)*ASSET_GROUP_COUNT;
  
  Sui_Header header = {};
  header.magic_value = SUI_MAGIC_VALUE;
  header.group_count = ASSET_GROUP_COUNT;
  header.asset_count = sp->asset_count;
  header.tag_count = sp->tag_count;
  header.offset_to_assets = sizeof(Sui_Header);
  header.offset_to_tags = header.offset_to_assets + asset_array_size;
  header.offset_to_groups = header.offset_to_tags + asset_tag_array_size;
  
  fwrite(&header, sizeof(header), 1, file);
  U32 offset_to_asset_data = asset_tag_array_size + asset_array_size + group_array_size;
  
  fseek(file, offset_to_asset_data, SEEK_CUR);
  
  // Skip 0 for null asset
  for(U32 i = 1; i < header.asset_count; ++i) {
    Sui_Asset* sui_asset = sp->assets + i;
    Karu_Packer_Source* source = sp->sources + i;
    
    sui_asset->offset_to_data = ftell(file);
    switch(sui_asset->type) {
      case ASSET_TYPE_BITMAP: {
        karu_log("writing bitmap\n");
        
        Sui_Bitmap sui_bitmap = {};
        sui_bitmap.width = source->bitmap.width;
        sui_bitmap.height = source->bitmap.height;
        fwrite(&sui_bitmap, sizeof(sui_bitmap), 1, file);
        
        U32 image_size = sui_bitmap.width * sui_bitmap.height * 4;
        fwrite(source->bitmap.pixels, image_size, 1, file);
        
      } break;
      case ASSET_TYPE_IMAGE: {
        karu_log("writing image\n");
        Sui_Image sui_image = {};
        sui_image.uv = source->image.uv;
        sui_image.bitmap_asset_id = source->image.bitmap_asset_id;
        fwrite(&sui_image, sizeof(sui_image), 1, file);
      }
    }
  }
  
  // Write metadata
  fseek(file, header.offset_to_assets, SEEK_SET);
  fwrite(sp->assets, asset_array_size, 1, file); 
  
  fseek(file, header.offset_to_groups, SEEK_SET);
  fwrite(sp->groups, group_array_size, 1, file); 
  
  fseek(file, header.offset_to_tags, SEEK_SET);
  fwrite(sp->tags, asset_tag_array_size, 1, file); 
  
}


#endif //KARU_PACK_H
