// Authors: Gerald Wong, momodevelop
// 
// This file processes TTF files:
// - Extracts font and glyph information
// - Performs rasterization
//
// Notes:
// - Only works in little-endian OS
// - Only reads and writes into 32-bit RGBA format
// - Prioritizes formats recognized by Windows first.
// 
// Todo:
// - Cater for glyphs which start from an off-curve point.
// - Anti-aliasing
// - Complex glyphs.
// - Kerning:
//   - gpos support
//   - other format support for kern (other than 0)
// - Different rasterization color modes

#ifndef MOMO_TTF_H
#define MOMO_TTF_H


struct TTF {
  U8* data;
  U32 glyph_count;
  
  // these are positions from data
  U32 loca, head, glyf, maxp, cmap, hhea, hmtx, kern, gpos;
  U32 cmap_mappings;
  
  U16 loca_format;
};

struct TTF_Glyph_Horizontal_Metrics
{
  S16 advance_width;
  S16 left_side_bearing;
};


static TTF read_ttf(Memory ttf_memory);

static U32 get_glyph_index_from_codepoint(TTF* ttf, U32 codepoint);
// returns 0 for invalid codepoints

static TTF_Glyph_Horizontal_Metrics get_glyph_horizontal_metrics(TTF* ttf, U32 glyph_index);

static F32 get_scale_for_pixel_height(TTF* ttf, F32 pixel_height);
// This returns the 'scale factor' you need to apply to the font's coordinates
// (box, glyphs, etc) to scale it to a font height equals to pixel_height


static Image rasterize_glyph(TTF* ttf, U32 glyph_index, F32 scale_factor, Arena* arena);
// Returns an RGBA image where the glyph is white and the background is transparent

static S32 get_glyph_kerning(TTF* ttf, U32 glyph_index_1, U32 glyph_index_2);

#include "momo_ttf.cpp"


#endif //MOMO_TTF_H
