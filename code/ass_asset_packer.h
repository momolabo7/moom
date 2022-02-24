/* date = February 21st 2022 7:42 pm */

#ifndef ASS_ASSET_PACKER_H
#define ASS_ASSET_PACKER_H



enum SUI_Asset_Source_Type {
  ASSET_SOURCE_TYPE_BITMAP,
};

struct SUI_Asset_Bitmap_Source {
  U32 width;
  U32 height;
  U32* pixels;
};

struct SUI_Asset_Image_Source {
  Asset_Bitmap_ID bitmap_id;
  Rect2 uv;
};

struct SUI_Font_Source {
  Atlaser_Font* ptr;
  Asset_Bitmap_ID bitmap_id;
};

struct SUI_Asset_Source {
  SUI_Asset_Source_Type type;
  union {
    SUI_Asset_Bitmap_Source bitmap;
    SUI_Asset_Image_Source image;
  };
};


struct SUI_Packer {
  U32 asset_count;
  SUI_Asset_Source sources[1024];
  SUI_Asset assets[1024];
  
	
  SUI_Asset_Group groups[ASSET_GROUP_COUNT];
  SUI_Asset_Group* active_group;
};

static SUI_Packer
begin_sui_packer() {
  
  SUI_Packer ret = {};
  //ret.asset_count = 0;
  ret.active_group = nullptr;
  return ret;
}


static void
begin_asset_group(SUI_Packer* sp, Asset_Group_ID asset_group_id) 
{
  sp->active_group = sp->groups + asset_group_id;
  sp->active_group->first_asset_id = sp->asset_count;
  sp->active_group->one_past_last_asset_id = sp->active_group->first_asset_id;
}

static void
end_asset_group(SUI_Packer* sp) 
{
  sp->active_group = nullptr;
}


static Asset_Bitmap_ID
add_bitmap_asset(SUI_Packer* sp, Bitmap bitmap) {
  assert(sp->active_group);
  ++sp->active_group->one_past_last_asset_id;
  
  U32 index = sp->asset_count++;
  
  SUI_Asset_Source* source = sp->sources + index;
  source->bitmap.width = bitmap.width;
  source->bitmap.height = bitmap.height;
  source->bitmap.pixels = bitmap.pixels;
  
  SUI_Asset* asset = sp->assets + index;
  asset->type = ASSET_TYPE_BITMAP;
  
  Asset_Bitmap_ID ret;
  ret.value = index;
  return ret;
}

static Asset_Image_ID
add_image_asset(SUI_Packer* sp, 
                Asset_Bitmap_ID bitmap_id,
                Rect2 uv)
{
  assert(sp->active_group);
  ++sp->active_group->one_past_last_asset_id;
  
  U32 index = sp->asset_count++;
  
  SUI_Asset_Source* source = sp->sources + index;
  source->image.bitmap_id = bitmap_id;
  source->image.uv = uv;
  
  SUI_Asset* asset = sp->assets + index;
  asset->type = ASSET_TYPE_IMAGE;
  
  Asset_Image_ID ret;
  ret.value = index;
  return ret;
  
}



static void
end_sui_packer(SUI_Packer* sp, const char* filename) {
  FILE* file = fopen(filename, "wb");
  defer { fclose(file); };
  
  U32 asset_array_size = sizeof(SUI_Asset)*sp->asset_count;
  U32 asset_group_array_size = sizeof(SUI_Asset_Group)*ASSET_GROUP_COUNT;
  
  SUI_Header header = {};
  header.magic_value = SUI_MAGIC_VALUE;
  header.asset_group_count = ASSET_GROUP_COUNT;
  header.asset_count = sp->asset_count;
  header.offset_to_assets = sizeof(SUI_Header);
  header.offset_to_asset_groups = header.offset_to_assets + asset_array_size;
  
  fwrite(&header, sizeof(header), 1, file);
  fseek(file, asset_array_size + asset_group_array_size, SEEK_CUR);
  
  for(U32 i = 0; i < header.asset_count; ++i) {
    SUI_Asset* sui_asset = sp->assets + i;
    SUI_Asset_Source* source = sp->sources + i;
    
    
    
    
    sui_asset->offset_to_data = ftell(file);
    switch(sui_asset->type) {
      case ASSET_TYPE_BITMAP: {
        ass_log("writing bitmap\n");
        
        SUI_Bitmap sui_bitmap = {};
        sui_bitmap.width = source->bitmap.width;
        sui_bitmap.height = source->bitmap.height;
        fwrite(&sui_bitmap, sizeof(sui_bitmap), 1, file);
        
        U32 image_size = sui_bitmap.width * sui_bitmap.height * 4;
        fwrite(source->bitmap.pixels, image_size, 1, file);
        
      } break;
      case ASSET_TYPE_IMAGE: {
        ass_log("writing image\n");
        SUI_Image sui_image = {};
        sui_image.uv = source->image.uv;
        sui_image.bitmap_id = source->image.bitmap_id;
        fwrite(&sui_image, sizeof(sui_image), 1, file);
        
      }
    }
    
  }
  
  fseek(file, header.offset_to_assets, SEEK_SET);
  fwrite(sp->assets, asset_array_size, 1, file); 
  
  fseek(file, header.offset_to_asset_groups, SEEK_SET);
  fwrite(sp->groups, asset_group_array_size, 1, file); 
  
}


#endif //ASS_ASSET_PACKER_H
