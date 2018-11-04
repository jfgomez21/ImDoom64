#include <sstream>
#include <map>

#include "wad/wad.hh"
#include "wad/lump_hash.hh"
#include "map.hh"

namespace {
  template <class T>
  void read_into(std::istream& s, T& x)
  {
      s.read(reinterpret_cast<char*>(&x), sizeof(T));
  }

  struct Header {
      char id[4];
      uint32 numlumps;
      uint32 infotableofs;
  };

  struct Directory {
      uint32 filepos;
      uint32 size;
      char name[8];
  };

  struct MapLump {
      String data;
  };

  std::vector<MapLump> lumps_;

  std::map<int, int> texturehashlist_;
}

void* W_GetMapLump(int lump)
{
    return &lumps_[lump].data[0];
}

void W_CacheMapLump(int map)
{
    auto file = wad::open(fmt::format("MAP{:02d}", map));

    if (!file) {
        log::fatal("Could not find MAP{:02d}", map);
    }

    lumps_.clear();

    auto& s = file->stream();

    Header header;
    read_into(s, header);

    if (memcmp(header.id, "IWAD", 4) != 0 && memcmp(header.id, "PWAD", 4)) {
        log::fatal("MAP{:02d} is an invalid WAD", map);
    }

    std::size_t numlumps = header.numlumps;
    s.seekg(header.infotableofs);
    for (std::size_t i = 0; !s.eof() && i < numlumps; ++i) {
        Directory dir;
        read_into(s, dir);

        auto pos = s.tellg();
        s.seekg(dir.filepos);
        String str(dir.size, 0);
        s.read(&str[0], dir.size);
        s.seekg(pos);

        lumps_.push_back({ std::move(str) });
    }
}

void W_FreeMapLump()
{
    // nop
}

int W_MapLumpLength(int lump)
{
    return lumps_.at(lump).data.size();
}

//
// P_InitTextureHashTable
//

extern Vector<String> rom_textures;
void P_InitTextureHashTable(void) {
    // FIXME: Collision between ROM textures and PWAD textures!
    for(auto& lump : wad::list_section(wad::Section::textures)) {
        auto [it, ok] = texturehashlist_.try_emplace(
            wad::LumpHash(lump->name()).get(),
            lump->section_index());

        if (!ok) {
            DEBUG("Texture '{:<8}' shares hash with '{:<8}'! ({})",
                  lump->name(),
                  wad::open(wad::Section::textures, it->second)->name(),
                  it->first);
        }
    }
    for (size_t i = 0; i < rom_textures.size(); ++i) {
        auto [it, ok] = texturehashlist_.try_emplace(i, i);

        if (!ok) {
            DEBUG("Texture '{:<8}' shares hash with '{:<8}'! ({})",
                  wad::open(wad::Section::textures, i)->name(),
                  wad::open(wad::Section::textures, it->second)->name(),
                  it->first);
        }
    }
}

//
// P_GetTextureHashKey
//

uint32 P_GetTextureHashKey(int hash) {
    auto it = texturehashlist_.find(hash);
    if (it == texturehashlist_.end()) {
        return 0;
    } else {
        return it->second;
    }
}
