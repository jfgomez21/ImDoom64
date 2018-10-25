#ifndef WAD_LOADERS_HH
#define WAD_LOADERS_HH

#include "idevice.hh"

namespace imp {
  namespace wad {
    IDevicePtr zip_loader(StringView);
    IDevicePtr doom_loader(StringView);
    IDevicePtr rom_loader(StringView);
  }
}

#endif //WAD_LOADERS_HH
