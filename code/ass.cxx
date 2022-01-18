// This is the asset builder tool
//
// Things I know the game wants:
// - wants to specifiy exactly what assets to

struct AssetPacker {
};

static AssetPacker
begin_asset_pack(const char* filename) {
  AssetPacker ret = {};
  return ret; 
}

static void
end_asset_pack(AssetPacker* packer) {
}

int main() {
  auto packer = begin_asset_pack("test");

  end_asset_pack(&packer);
}
