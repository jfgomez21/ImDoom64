#ifndef __MAP_LUMP__24203468
#define __MAP_LUMP__24203468

#include "wad/ilump.hh"

namespace imp::wad::doom {
  class MapLump : public ILump {
      struct Lump {
          char name[8] {};
          size_t siz;
          size_t pos;

          Lump(std::string_view name, size_t siz, size_t pos):
              siz(siz),
              pos(pos)
          {
              std::fill_n(this->name, 8, 0);
              std::copy_n(name.data(), std::min(name.size(), static_cast<size_t>(8)), this->name);
          }
      };

      static constexpr size_t table_pos = 8;

      IDevice& m_device;
      std::string m_name {};
      std::vector<Lump> m_lumps {};
      std::string m_data {};

  public:
      MapLump(IDevice& device, const std::string& name):
          m_device(device),
          m_name(name),
          m_data("PWAD\x0e\0\0\0\0\0\0\0"s)
      {}

      void add_lump(std::string_view name, const std::string& lump)
      {
          m_lumps.emplace_back(name, lump.size(), m_data.size());
          m_data.append(lump);
      }

      void build()
      {
          m_data[table_pos + 3] = (m_data.size() >> 24) & 0xff;
          m_data[table_pos + 2] = (m_data.size() >> 16) & 0xff;
          m_data[table_pos + 1] = (m_data.size() >> 8)  & 0xff;
          m_data[table_pos + 0] = (m_data.size())       & 0xff;

          for (const auto& l : m_lumps) {
              m_data.push_back((l.pos) & 0xff);
              m_data.push_back((l.pos >> 8) & 0xff);
              m_data.push_back((l.pos >> 16) & 0xff);
              m_data.push_back((l.pos >> 24) & 0xff);

              m_data.push_back((l.siz) & 0xff);
              m_data.push_back((l.siz >> 8) & 0xff);
              m_data.push_back((l.siz >> 16) & 0xff);
              m_data.push_back((l.siz >> 24) & 0xff);

              m_data.append(l.name, l.name + 8);
          }

          m_lumps.clear();
      }

      String name() const override
      { return m_name; }

      Section section() const override
      { return Section::normal; }

      String real_name() const override
      { return m_name; }

      IDevice& device() override
      { return m_device; }

      UniquePtr<std::istream> stream() override
      { return std::make_unique<std::istringstream>(m_data); }
  };
}

#endif //__MAP_LUMP__24203468
