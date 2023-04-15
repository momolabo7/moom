// moe_asset_types.h
enum Asset_Tag_Type : u32_t {
  ASSET_TAG_TYPE_FONT,

  ASSET_TAG_TYPE_COUNT,
};

enum Asset_Group_Type : u32_t {
  ASSET_GROUP_TYPE_BLANK_SPRITE,
  ASSET_GROUP_TYPE_CIRCLE_SPRITE,
  ASSET_GROUP_TYPE_FILLED_CIRCLE_SPRITE,
  ASSET_GROUP_TYPE_MOVE_SPRITE,
  ASSET_GROUP_TYPE_ROTATE_SPRITE,

  ASSET_GROUP_TYPE_ATLAS,
  ASSET_GROUP_TYPE_FONTS,

  ASSET_GROUP_TYPE_COUNT,
};

enum Asset_Type : u32_t {
  ASSET_TYPE_SPRITE,
  ASSET_TYPE_FONT,
  ASSET_TYPE_BITMAP,
};
