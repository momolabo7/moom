/* date = February 21st 2022 7:42 pm */

#ifndef ASS_ASSET_PACKER_H
#define ASS_ASSET_PACKER_H



enum SUI_Packer_Source_Type {
  SUI_PACKER_BITMAP_SOURCE,
};

struct SUI_Packer_Bitmap_Source {
  
};

struct SUI_Packer_Source {
  SUI_Packer_Source_Type type;
  union {
    SUI_Packer_Bitmap_Source bitmap;
  };
};


struct SUI_Packer {
  U32 tag_count;
  SUI_Tag tags[1024];
  
  U32 source_count;    
  SUI_Packer_Source sources[1024];
  
  U32 asset_count;
  SUI_Asset assets[1024];
  
  U32 asset_type_count;
  Asset_Type asset_types[ASSET_COUNT];
  
};

static SUI_Packer
create_asset_packer() {
}

static void 
begin_asset_type(SUI_Packer* ap, Asset_Type type) {
  
}

static void
add_font_asset(SUI_Packer* ap) {
}

static void
add_bitmap_asset(SUI_Packer* ap, Bitmap* bitmap) {
}

static void
add_image_asset(SUI_Packer* ap) {
  
}

static void 
end_asset_type() {
  
}


#endif //ASS_ASSET_PACKER_H
