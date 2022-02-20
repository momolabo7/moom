/* date = February 20th 2022 10:10 pm */

#ifndef GAME_ASSET_FILE_H
#define GAME_ASSET_FILE_H

constexpr char* atlas_file_signature = "atlas";

#pragma pack(push,1)
// Atlas data will be in RGBA format.
// All these are stored in little endian format
// (which makes this struct useful!)
struct Atlas_File_Header {
  U32 rect_count;
  U32 atlas_width;
  U32 atlas_height;
  
  // followed by:
  // Rect2 rects[rect_count] 
  // U32 pixels[atlas_width*atlas_height]
};
#pragma pack(pop)


#endif //GAME_ASSET_FILE_H
