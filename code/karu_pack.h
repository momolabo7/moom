/* date = February 21st 2022 7:42 pm */

#ifndef KARU_PACK_H
#define KARU_PACK_H

enum Karu_Source_Type{
  KARU_SOURCE_TYPE_BITMAP,
  KARU_SOURCE_TYPE_IMAGE,
  KARU_SOURCE_TYPE_ATLAS_FONT,
};

struct Karu_Bitmap_Source {
  U32 width;
  U32 height;
  U32* pixels;
};

struct Karu_Image_Source {
  U32 bitmap_asset_id;
  Rect2 uv;
};


struct Karu_Atlas_Font_Source {
  Karu_Atlas* atlas;
  U32 atlas_font_id;
  U32 bitmap_asset_id;
};

struct Karu_Source {
  Karu_Source_Type type;
  union {
    Karu_Bitmap_Source bitmap;
    Karu_Image_Source image;
    Karu_Atlas_Font_Source atlas_font;
  };
};


struct Karu_Packer {
  U32 tag_count;
  Sui_Tag tags[1024]; // to be written to file
  
  U32 asset_count;
  Karu_Source sources[1024]; // additional data for assets
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

struct _Karu_Packer_Added_Entry {
  U32 asset_index;
  Karu_Source* source;
};

static _Karu_Packer_Added_Entry
_add_asset(Karu_Packer* sp, Karu_Source_Type type) {
  assert(sp->active_group);
  U32 asset_index = sp->asset_count++;
  ++sp->active_group->one_past_last_asset_index;
  sp->active_asset_index = asset_index;
  
  Sui_Asset* asset = sp->assets + asset_index;
  asset->first_tag_index = sp->tag_count;
  asset->one_past_last_tag_index = asset->one_past_last_tag_index;
  
  Karu_Source* source = sp->sources + asset_index;
  source->type = type;
  
  _Karu_Packer_Added_Entry ret;
  ret.source = source;
  ret.asset_index = asset_index;
  
  return ret;
}

static U32
add_font(Karu_Packer* sp, 
         U32 bitmap_asset_id, 
         Karu_Atlas* atlas,
         U32 atlas_font_id) 
{
  auto added_asset = _add_asset(sp, KARU_SOURCE_TYPE_ATLAS_FONT); 
  added_asset.source->atlas_font.atlas = atlas;
  added_asset.source->atlas_font.atlas_font_id = atlas_font_id;
  added_asset.source->atlas_font.bitmap_asset_id = bitmap_asset_id;  
  
  return added_asset.asset_index;
}

static U32
add_bitmap(Karu_Packer* sp, Bitmap bitmap) {
  auto added_asset = _add_asset(sp, KARU_SOURCE_TYPE_BITMAP);
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
  auto added_asset = _add_asset(sp, KARU_SOURCE_TYPE_IMAGE);
  added_asset.source->image.bitmap_asset_id = bitmap_asset_id;
  added_asset.source->image.uv = uv;
  
  return added_asset.asset_index;
}


static U32
add_image(Karu_Packer* sp, 
          U32 bitmap_asset_id,
          Karu_Atlas* atlas,
          U32 atlas_image_id)
{
  Karu_Atlas_Image* img = atlas->images + atlas_image_id;
  
  Rect2 uv = {};
  uv.min.x = (F32)img->rect->x / atlas->bitmap.width;
  uv.min.y = (F32)img->rect->y / atlas->bitmap.height;
  uv.max.x = (F32)(img->rect->x+img->rect->w) / atlas->bitmap.width;
  uv.max.y = (F32)(img->rect->y+img->rect->h) / atlas->bitmap.height;
  
  return add_image(sp, bitmap_asset_id, uv);
  
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
    Karu_Source* source = sp->sources + i;
    
    sui_asset->offset_to_data = ftell(file);
    switch(source->type) {
      case KARU_SOURCE_TYPE_BITMAP: {
        karu_log("writing bitmap\n");
        sui_asset->type = ASSET_TYPE_BITMAP;
        
        Sui_Bitmap sui_bitmap = {};
        sui_bitmap.width = source->bitmap.width;
        sui_bitmap.height = source->bitmap.height;
        fwrite(&sui_bitmap, sizeof(sui_bitmap), 1, file);
        
        U32 image_size = sui_bitmap.width * sui_bitmap.height * 4;
        fwrite(source->bitmap.pixels, image_size, 1, file);
        
      } break;
      case KARU_SOURCE_TYPE_IMAGE: {
        karu_log("writing image\n");
        sui_asset->type = ASSET_TYPE_IMAGE;
        
        Sui_Image sui_image = {};
        sui_image.uv = source->image.uv;
        sui_image.bitmap_asset_id = source->image.bitmap_asset_id;
        fwrite(&sui_image, sizeof(sui_image), 1, file);
      } break;
      case ASSET_TYPE_FONT: {
        karu_log("writing font\n");
        
      } break;
      
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
