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
  //wU32 asset_count;
  SUI_Asset_Source sources[ASSET_COUNT];
  SUI_Asset_Header asset_headers[ASSET_COUNT];
  
};

static SUI_Packer
begin_sui_packer() {
  
  SUI_Packer ret = {};
  //ret.asset_count = 0;
  
  return ret;
}

static Asset_Bitmap_ID
add_bitmap_asset(SUI_Packer* sp, Asset_ID id, Bitmap bitmap) {
  SUI_Asset_Source* source = sp->sources + id;
  source->bitmap.width = bitmap.width;
  source->bitmap.height = bitmap.height;
  source->bitmap.pixels = bitmap.pixels;
  
  SUI_Asset_Header* header = sp->asset_headers + id;
  header->type = ASSET_TYPE_BITMAP;
  
  Asset_Bitmap_ID ret;
  ret.value = id;
  return ret;
}

static void
add_image_asset(SUI_Packer* sp, 
                Asset_ID id,
                Asset_Bitmap_ID bitmap_id,
                Rect2 uv)
{
  SUI_Asset_Source* source = sp->sources + id;
  source->image.bitmap_id = bitmap_id;
  source->image.uv = uv;
  
  SUI_Asset_Header* header = sp->asset_headers + id;
  header->type = ASSET_TYPE_IMAGE;
  
}


static void 
end_asset_group(SUI_Packer* sp) {
  
}


static void
end_sui_packer(SUI_Packer* sp, const char* filename) {
  FILE* file = fopen(filename, "wb");
  defer { fclose(file); };
  
  SUI_Header header = {};
  header.magic_value = SUI_MAGIC_VALUE;
  header.asset_count = ASSET_COUNT; //sp->asset_count;
  header.offset_to_asset_headers = sizeof(SUI_Header);
  
  fwrite(&header, sizeof(header), 1, file);
  
  U32 asset_header_size = sizeof(SUI_Asset_Header)*header.asset_count;
  fseek(file, asset_header_size, SEEK_CUR);
  
  for(U32 i = 0; i < header.asset_count; ++i) {
    SUI_Asset_Header* asset_header = sp->asset_headers + i;
    SUI_Asset_Source* source = sp->sources + i;
    
    asset_header->offset_to_data = ftell(file);
    switch(asset_header->type) {
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
  
  fseek(file, header.offset_to_asset_headers, SEEK_SET);
  
  fwrite(&sp->asset_headers, asset_header_size, 1, file); 
  
}


#endif //ASS_ASSET_PACKER_H
