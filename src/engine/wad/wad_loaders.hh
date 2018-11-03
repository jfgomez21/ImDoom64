#ifndef WAD_LOADERS_HH
#define WAD_LOADERS_HH

#include "idevice.hh"
#include <filesystem>

namespace imp {
  namespace wad {
    IDevicePtr zip_loader(const std::filesystem::path&);
    IDevicePtr doom_loader(const std::filesystem::path&);
    IDevicePtr rom_loader(const std::filesystem::path&);
  }
}

#endif //WAD_LOADERS_HH
