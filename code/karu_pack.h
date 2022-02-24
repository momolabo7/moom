/* date = February 21st 2022 7:42 pm */

#ifndef KARU_PACK_H
#define KARU_PACK_H


struct Karu_Packer_Bitmap_Source {
  U32 width;
  U32 height;
  U32* pixels;
};

struct Karu_Packer_Image_Source {
  Asset_Bitmap_ID bitmap_id;
  Rect2 uv;
};

struct Karu_Packer_Font_Source {
  Karu_Atlas_Font* ptr;
  Asset_Bitmap_ID bitmap_id;
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
begin_asset_group(Karu_Packer* sp, Asset_Group_ID asset_group_id) 
{
  sp->active_group = sp->groups + asset_group_id;
  sp->active_group->first_asset_id = Asset_ID{ sp->asset_count };
  sp->active_group->one_past_last_asset_id = sp->active_group->first_asset_id;
}

static void
end_asset_group(Karu_Packer* sp) 
{
  sp->active_group = nullptr;
}


static Asset_Bitmap_ID
add_bitmap_asset(Karu_Packer* sp, Bitmap bitmap) {
  assert(sp->active_group);
  ++sp->active_group->one_past_last_asset_id.value;
  sp->active_asset_index = sp->asset_count++;
  
  Karu_Packer_Source* source = sp->sources + sp->active_asset_index;
  source->bitmap.width = bitmap.width;
  source->bitmap.height = bitmap.height;
  source->bitmap.pixels = bitmap.pixels;
  
  Sui_Asset* asset = sp->assets + sp->active_asset_index;
  asset->type = ASSET_TYPE_BITMAP;
  asset->first_tag_index = sp->tag_count;
  asset->one_past_last_tag_index = asset->one_past_last_tag_index;
  
  
  Asset_Bitmap_ID ret;
  ret.value = sp->active_asset_index;
  return ret;
}


static Asset_Image_ID
add_image_asset(Karu_Packer* sp, 
                Asset_Bitmap_ID bitmap_id,
                Rect2 uv)
{
  assert(sp->active_group);
  ++sp->active_group->one_past_last_asset_id.value;
  sp->active_asset_index = sp->asset_count++;
  
  Karu_Packer_Source* source = sp->sources + sp->active_asset_index;
  source->image.bitmap_id = bitmap_id;
  source->image.uv = uv;
  
  Sui_Asset* asset = sp->assets + sp->active_asset_index;
  asset->type = ASSET_TYPE_IMAGE;
  asset->first_tag_index = sp->tag_count;
  asset->one_past_last_tag_index = asset->one_past_last_tag_index;
  
  
  Asset_Image_ID ret;
  ret.value = sp->active_asset_index;
  return ret;
  
}



static void
end_sui_packer(Karu_Packer* sp, const char* filename) {
  FILE* file = fopen(filename, "wb");
  defer { fclose(file); };
  
  U32 asset_tag_array_size = sizeof(Sui_Tag)*sp->tag_count;
  U32 asset_array_size = sizeof(Sui_Asset)*sp->asset_count;
  U32 asset_group_array_size = sizeof(Sui_Asset_Group)*ASSET_GROUP_COUNT;
  
  Sui_Header header = {};
  header.magic_value = Sui_MAGIC_VALUE;
  header.asset_group_count = ASSET_GROUP_COUNT;
  header.asset_count = sp->asset_count;
  header.tag_count = sp->tag_count;
  header.offset_to_assets = sizeof(Sui_Header);
  header.offset_to_tags = header.offset_to_assets + asset_array_size;
  header.offset_to_asset_groups = header.offset_to_tags + asset_tag_array_size;
  
  fwrite(&header, sizeof(header), 1, file);
  U32 offset_to_asset_data = asset_tag_array_size + asset_array_size + asset_group_array_size;
  
  fseek(file, offset_to_asset_data, SEEK_CUR);
  
  for(U32 i = 0; i < header.asset_count; ++i) {
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
        sui_image.bitmap_id = source->image.bitmap_id;
        fwrite(&sui_image, sizeof(sui_image), 1, file);
      }
    }
  }
  
  // Write metadata
  fseek(file, header.offset_to_assets, SEEK_SET);
  fwrite(sp->assets, asset_array_size, 1, file); 
  
  fseek(file, header.offset_to_asset_groups, SEEK_SET);
  fwrite(sp->groups, asset_group_array_size, 1, file); 
  
  fseek(file, header.offset_to_tags, SEEK_SET);
  fwrite(sp->tags, asset_tag_array_size, 1, file); 
  
}


#endif //KARU_PACK_H
