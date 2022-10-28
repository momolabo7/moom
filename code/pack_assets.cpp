#include <stdlib.h>
#include <stdio.h>

#include "sui.h"
#include "sui_atlas.h"

static Rect2U
sui_rp_rect_to_rect2u(RP_Rect rp) {
  Rect2U ret = {0};
  ret.min.x = rp.x;
  ret.min.y = rp.y;
  ret.max.x = rp.x + rp.w;
  ret.max.y = rp.y + rp.h;

  return ret;
}

// game_asset_types.h
enum Game_Asset_Tag : U32 {
  GAME_ASSET_TAG_MOOD,

  GAME_ASSET_TAG_COUNT,
};
enum Game_Asset_Group : U32 {
  GAME_ASSET_GROUP_TEST,
  
  GAME_ASSET_GROUP_COUNT,
};

enum Game_Asset_Type : U32 {
  GAME_ASSET_TYPE_SPRITE,
  GAME_ASSET_TYPE_FONT,
  GAME_ASSET_TYPE_BITMAP,
};

// karu.h
#define KARU_CODE(a, b, c, d) (((U32)(a) << 0) | ((U32)(b) << 8) | ((U32)(c) << 16) | ((U32)(d) << 24))
#define KARU_SIGNATURE KARU_CODE('k', 'a', 'r', 'u')


struct Karu_Bitmap {
  U32 width;
  U32 height;
  
  // data:
  //   U32 pixels[width*height]
};

struct Karu_Font_Glyph {
  U32 bitmap_asset_id; 
  Rect2U texel_uv;
  Rect2 box;
  U32 codepoint;
};

struct Karu_Font {
  U32 offset_to_data;
  
  // TODO: Maybe add 'lowest codepoint'?
  U32 bitmap_asset_id;
  U32 highest_codepoint;
  U32 glyph_count;
  
  // Data is: 
  // 
  // Karu_Font_Glyph glyphs[glyph_count]
  // F32 horizontal_advances[glyph_count][glyph_count]
  //

};

struct Karu_Sprite {
  U32 bitmap_asset_id; 
  Rect2U texel_uv;
};

struct Karu_Asset {
  Game_Asset_Type type; 

  U32 offset_to_data;

  // Tag info
  U32 first_tag_index;
  U32 one_past_last_tag_index;

  union {
    Karu_Bitmap bitmap;
    Karu_Font font;
    Karu_Sprite sprite;
  };
};

struct Karu_Group {
  U32 first_asset_index;
  U32 one_past_last_asset_index;
};

struct Karu_Tag {
  Game_Asset_Tag type; 
  F32 value;
};


// sui_packer.h
enum Sui_Packer_Source_Type {
  SUI_PACKER_SOURCE_TYPE_BITMAP,
  SUI_PACKER_SOURCE_TYPE_SPRITE,
  SUI_PACKER_SOURCE_TYPE_FONT,
};

struct Sui_Packer_Source_Sprite {
  U32 bitmap_asset_id;
  Rect2U texel_uv; 
};

struct Sui_Packer_Source_Bitmap {
  U32 width;
  U32 height;
  U32* pixels;
};

struct Sui_Packer_Source_Font {
  U32 bitmap_asset_id;
  Sui_Atlas_Font* atlas_font;
};

struct Sui_Packer_Source {
  Sui_Packer_Source_Type type;
  union {
    Sui_Packer_Source_Sprite sprite; 
    Sui_Packer_Source_Bitmap bitmap; 
    Sui_Packer_Source_Font font;
  };
};

struct Sui_Packer {
  U32 tag_count;
  Karu_Tag tags[1024]; // to be written to file
  
  U32 asset_count;
  Sui_Packer_Source sources[1024]; // additional data for assets
  Karu_Asset assets[1024]; // to be written to file
  
  Karu_Group groups[GAME_ASSET_GROUP_COUNT]; //to be written to file
  
  // Required context for interface
  Karu_Group* active_group;
  U32 active_asset_index;
};

static void
sui_pack_begin(Sui_Packer* p) {
  p->asset_count = 1; // reserve for null asset
  p->tag_count = 1;   // reserve for null tag
}

static void
sui_pack_add_tag(Sui_Packer* p, Game_Asset_Tag tag_type, F32 value) {
  U32 tag_index = p->tag_count++;
  
  Karu_Asset* asset = p->assets + p->active_asset_index;
  asset->one_past_last_tag_index = p->tag_count;
  
  Karu_Tag* tag = p->tags + tag_index;
  tag->type = tag_type;
  tag->value = value;
}

static void
sui_pack_begin_group(Sui_Packer* p, Game_Asset_Group group) 
{
  p->active_group = p->groups + group;
  p->active_group->first_asset_index = p->asset_count;
  p->active_group->one_past_last_asset_index = p->active_group->first_asset_index;
}

static void
sui_pack_end_group(Sui_Packer* p) 
{
  p->active_group = null;
}

static void
sui_pack_push_sprite(Sui_Packer* p, U32 bitmap_asset_id, Rect2U texel_uv) {
  assert(p->active_group);
  assert(p->asset_count < array_count(p->assets));

  U32 asset_index = p->asset_count++;
  ++p->active_group->one_past_last_asset_index;
  p->active_asset_index = asset_index;

  Karu_Asset* asset = p->assets + asset_index;
  asset->first_tag_index = p->tag_count;
  asset->one_past_last_tag_index = asset->first_tag_index;

  Sui_Packer_Source* source = p->sources + asset_index;
  source->type = SUI_PACKER_SOURCE_TYPE_SPRITE;

  source->sprite.bitmap_asset_id = bitmap_asset_id;
  source->sprite.texel_uv = texel_uv;

  //Karu_Sprite* sprite = &asset->sprite;
  
}

// TODO: return something else?
static U32
sui_pack_push_bitmap(Sui_Packer* p, U32 width, U32 height, U32* pixels) {
  assert(p->active_group);
  assert(p->asset_count < array_count(p->assets));

  U32 asset_index = p->asset_count++;
  ++p->active_group->one_past_last_asset_index;
  p->active_asset_index = asset_index;

  Karu_Asset* asset = p->assets + asset_index;
  asset->first_tag_index = p->tag_count;
  asset->one_past_last_tag_index = asset->first_tag_index;

  Sui_Packer_Source* source = p->sources + asset_index;
  source->type = SUI_PACKER_SOURCE_TYPE_BITMAP;

  source->bitmap.width = width;
  source->bitmap.height = height;
  source->bitmap.pixels = pixels;

  return asset_index;
}

// TODO: return something else?
static U32
sui_pack_push_font(Sui_Packer* p, Sui_Atlas_Font* font, U32 bitmap_asset_id) {
  assert(p->active_group);
  assert(p->asset_count < array_count(p->assets));

  U32 asset_index = p->asset_count++;
  ++p->active_group->one_past_last_asset_index;
  p->active_asset_index = asset_index;

  Karu_Asset* asset = p->assets + asset_index;
  asset->first_tag_index = p->tag_count;
  asset->one_past_last_tag_index = asset->first_tag_index;

  Sui_Packer_Source* source = p->sources + asset_index;
  source->type = SUI_PACKER_SOURCE_TYPE_FONT;

  source->font.atlas_font = font;
  source->font.bitmap_asset_id = bitmap_asset_id;

  return asset_index;
}

struct Karu_Header {
  U32 signature;

  U32 group_count;
  U32 asset_count;
  U32 tag_count;

  U32 offset_to_assets;
  U32 offset_to_tags;
  U32 offset_to_groups;
};

static void
sui_pack_end(Sui_Packer* p, const char* filename, Bump_Allocator* arena) 
{
  FILE* file = fopen(filename, "wb");
  defer { fclose (file); };
 
  U32 asset_tag_array_size = sizeof(Karu_Tag)*p->tag_count;
  U32 asset_array_size = sizeof(Karu_Asset)*p->asset_count;
  U32 group_array_size = sizeof(Karu_Group)*GAME_ASSET_GROUP_COUNT;

  Karu_Header header = {0};
  header.signature = KARU_SIGNATURE;
  header.group_count = GAME_ASSET_GROUP_COUNT;
  header.asset_count = p->asset_count;
  header.tag_count = p->tag_count;
  header.offset_to_assets = sizeof(Karu_Header);
  header.offset_to_tags = header.offset_to_assets + asset_array_size;
  header.offset_to_groups = header.offset_to_tags + asset_tag_array_size;
  fwrite(&header, sizeof(header), 1, file);

  U32 offset_to_asset_data = asset_tag_array_size + asset_array_size + group_array_size;

  fseek(file, offset_to_asset_data, SEEK_CUR);
  for (U32 asset_index = 1; asset_index < header.asset_count; ++asset_index) {
    Karu_Asset* asset = p->assets + asset_index;
    Sui_Packer_Source* source = p->sources + asset_index;

    // Write all 'extra' data of the assets
    asset->offset_to_data = ftell(file);
    switch(source->type) {
      case SUI_PACKER_SOURCE_TYPE_BITMAP:{
        sui_log("Writing bitmap\n");
        asset->type = GAME_ASSET_TYPE_BITMAP;

        Karu_Bitmap* bitmap = &asset->bitmap;
        bitmap->width = source->bitmap.width;
        bitmap->height = source->bitmap.height;
        
        U32 image_size = bitmap->width * bitmap->height * 4;
        fwrite(source->bitmap.pixels, image_size, 1, file);

      } break;
      case SUI_PACKER_SOURCE_TYPE_SPRITE: {
        sui_log("Writing sprite\n");

        asset->type = GAME_ASSET_TYPE_SPRITE;

        Karu_Sprite* sprite = &asset->sprite;
        sprite->bitmap_asset_id = source->sprite.bitmap_asset_id;
        sprite->texel_uv = source->sprite.texel_uv;
  
      } break;
      case SUI_PACKER_SOURCE_TYPE_FONT: {
        sui_log("Writing font\n");
        asset->type = GAME_ASSET_TYPE_FONT;

        Sui_Atlas_Font* atlas_font = source->font.atlas_font;

        make(TTF, ttf);
        B32 ok = sui_read_font_from_file(ttf, atlas_font->filename, arena); 
        assert(ok);
        
        // Figure out the highest codepoint
        U32 highest_codepoint = 0;
        for (U32 codepoint_index = 0; 
             codepoint_index < atlas_font->codepoint_count;
             ++codepoint_index) 
        {
          U32 codepoint = atlas_font->codepoints[codepoint_index];
          if(codepoint > highest_codepoint) {
            highest_codepoint = codepoint;
          }
        }
        if (highest_codepoint == 0) 
          continue;

        Karu_Font* font = &asset->font;
        font->highest_codepoint = highest_codepoint;
        font->glyph_count = atlas_font->codepoint_count;
        font->bitmap_asset_id = source->font.bitmap_asset_id;
        
        // push glyphs
        for (U32 rect_index = 0;
             rect_index < atlas_font->rect_count;
             ++rect_index) 
        {
          auto* glyph_rect = atlas_font->glyph_rects + rect_index;
          auto* glyph_rect_context = atlas_font->glyph_rect_contexts + rect_index;
          
          Karu_Font_Glyph glyph = {0};
          glyph.bitmap_asset_id = source->font.bitmap_asset_id;
          glyph.codepoint = glyph_rect_context->font_glyph.codepoint;
                   
          glyph.texel_uv = sui_rp_rect_to_rect2u(*glyph_rect);
          fwrite(&glyph, sizeof(glyph), 1, file);
        }
      } break;
    }
  }

  // Write metadata
  fseek(file, header.offset_to_assets, SEEK_SET);
  fwrite(p->assets, asset_array_size, 1, file); 
  
  fseek(file, header.offset_to_groups, SEEK_SET);
  fwrite(p->groups, group_array_size, 1, file); 
  
  fseek(file, header.offset_to_tags, SEEK_SET);
  fwrite(p->tags, asset_tag_array_size, 1, file); 

}


int main() {
  Block block = sui_malloc(MB(100));
  defer { sui_free(&block); };
  make(Bump_Allocator, allocator);
  ba_init(allocator, block.data, block.size);

  sui_log("Building atlas...\n");
  make(Sui_Atlas, atlas);

  sui_atlas_begin(atlas, 1024, 1024);
  Sui_Atlas_Sprite* blank_sprite = sui_atlas_push_sprite(atlas, sui_asset_dir("blank.png"));
  Sui_Atlas_Sprite* circle_sprite = sui_atlas_push_sprite(atlas, sui_asset_dir("circle.png"));
    
  sui_atlas_begin_font(atlas, sui_asset_dir("nokiafc22.ttf"), 72.f);
  for (U32 i = 32; i <= 126; ++i){
    sui_atlas_push_font_codepoint(atlas, i);
  }
  auto* font_a = sui_atlas_end_font(atlas);

  sui_atlas_begin_font(atlas, sui_asset_dir("liberation-mono.ttf"), 72.f);
  for (U32 i = 32; i <= 126; ++i){
    sui_atlas_push_font_codepoint(atlas, i);
  }
  auto* font_b = sui_atlas_end_font(atlas);
  sui_atlas_end(atlas, allocator);
  sui_log("Finished atlas...\n");

#if 1
  sui_log("Writing test png file...\n");
  Block png_to_write_memory = png_write_img32_to_blk(atlas->bitmap, allocator);
  sui_write_file_from_blk("test.png", png_to_write_memory);
#endif

  make(Sui_Packer, packer);
  sui_pack_begin(packer);

  sui_pack_begin_group(packer, GAME_ASSET_GROUP_TEST);
  U32 bitmap_id = sui_pack_push_bitmap(packer, atlas->bitmap.width, atlas->bitmap.height, atlas->bitmap.pixels);
  sui_pack_push_sprite(packer, bitmap_id, sui_rp_rect_to_rect2u(*blank_sprite->rect));
  sui_pack_push_sprite(packer, bitmap_id, sui_rp_rect_to_rect2u(*circle_sprite->rect));
  sui_pack_push_font(packer, font_a, bitmap_id);
  sui_pack_push_font(packer, font_b, bitmap_id);
  sui_pack_end_group(packer);

  sui_pack_end(packer, "test_pack.sui", allocator);
}

