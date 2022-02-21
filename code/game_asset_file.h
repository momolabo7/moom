/* date = February 20th 2022 10:10 pm */

#ifndef GAME_ASSET_FILE_H
#define GAME_ASSET_FILE_H

enum Game_Image_ID {
  GAME_IMAGE_BULLET_CIRCLE,
  GAME_IMAGE_BULLET_DOT,
  GAME_IMAGE_PLAYER_WHITE,
  GAME_IMAGE_PLAYER_BLACK
};

enum Game_Font_ID {
  GAME_FONT_DEFAULT,
};

enum Game_Bitmap_ID {
  GAME_BITMAP_DEFAULT,
};

enum Game_Asset_Type {
  ASSET_BITMAP,
  ASSET_IMAGE,
  ASSET_FONT,
};

#pragma pack(push,1)
struct Ass_Header {
  U32 asset_count;
  
};



struct Ass_Font_Glyph {
  Game_Asset_Type type;
  
  U32 codepoint;
  Rect2 uv;
  Game_Bitmap_ID bitmap_id;
};

struct Ass_Image {
  Game_Asset_Type type;
  
  Game_Image_ID id;
  Rect2 uv;
  Game_Bitmap_ID bitmap_id;
  
};

struct Ass_Bitmap {
  Game_Asset_Type type;
  
  Game_Bitmap_ID id;
  U32 width, height;
  // Data is:
  //
  // U8 pixels[width*height*4]
  //
};

#pragma pack(pop)


#endif //GAME_ASSET_FILE_H
