#include <map>
#include <algorithm>
#include <unordered_map>

#include "platform/app.hh"
#include "image/image.hh"
#include "wad/wad.hh"

using namespace imp::wad;

namespace {
  Vector<IDeviceLoader*> device_loaders_;

  bool dirty_ {};

  Vector<IDevicePtr> devices_;

  class SectionLumps {
      std::vector<ILumpPtr> m_lumps;
      std::unordered_map<String, size_t> m_index_by_name;

  public:
      using iterator = typename std::vector<ILumpPtr>::iterator;

      void push_back(ILumpPtr&& lump)
      {
          auto it = m_index_by_name.find(lump->name());
          if (lump->name() == "?" || it == m_index_by_name.end()) {
              lump->set_section_index(m_lumps.size());
              m_index_by_name.emplace(lump->name(), m_lumps.size());
              m_lumps.push_back(std::move(lump));
          } else {
              lump->set_previous(std::move(m_lumps[it->second]));
              m_lumps[it->second] = std::move(lump);
          }
      }

      std::pair<ILump*, size_t> find(StringView name)
      {
          auto it = m_index_by_name.find(std::string{name});
          if (it == m_index_by_name.end())
              return { nullptr, 0 };
          return { m_lumps[it->second].get(), it->second };
      }

      ILump* operator[](size_t index)
      {
          if (index >= m_lumps.size()) {
              return nullptr;
          }
          return m_lumps[index].get();
      }

      iterator begin()
      { return m_lumps.begin(); }

      iterator end()
      { return m_lumps.end(); }

      ArrayView<ILumpPtr> view()
      { return m_lumps; }
  };

  Array<SectionLumps, num_sections> section_lumps_;
}

bool wad::add_device(IDevicePtr device)
{
    auto lumps = device->read_all();
    for (auto& lump : lumps) {
        auto& list = section_lumps_[static_cast<size_t>(lump->section())];
        list.push_back(std::move(lump));
    }

    log::info("Added {} lumps from '{}'", lumps.size(), "");

    devices_.emplace_back(std::move(device));

    return true;
}

bool wad::add_device(StringView path)
{
    bool found {};
    for (auto l : device_loaders_) {
        if (auto d = l(path)) {
            found = add_device(std::move(d));
            break;
        }
    }

    if (!found) {
        log::error("Could not find an appropriate device loader for '{}'", path);
        return false;
    }

    return true;
}

bool wad::add_device_loader(IDeviceLoader& device_loader)
{
    device_loaders_.emplace_back(&device_loader);
    return false;
}

void wad::merge()
{
    size_t index {};
    for (auto& section : section_lumps_) {
        for (auto& lump : section) {
            lump->set_lump_index(index++);
        }
    }
    dirty_ = false;
}

Optional<Lump> wad::open(Section section, StringView name)
{
    auto& lumps = section_lumps_[static_cast<size_t>(section)];

    auto lump = lumps.find(name);
    if (!lump.first) {
        return nullopt;
    }

    return make_optional<Lump>(*lump.first);
}

Optional<Lump> wad::open(Section section, size_t index)
{
    auto& lumps = section_lumps_[static_cast<size_t>(section)];

    auto lump = lumps[index];
    if (!lump)
        return nullopt;
    return make_optional<Lump>(*lump);
}

Optional<Lump> wad::open(size_t index)
{
    assert(!dirty_);

    for (size_t i {}; i < num_sections; ++i) {
        auto &lumps = section_lumps_[i];

        for (auto &lump : lumps) {
            if (lump->lump_index() == index) {
                return make_optional<Lump>(*lump);
            }
        }
    }

    return nullopt;
}

ArrayView<ILumpPtr> wad::list_section(wad::Section section)
{
    return section_lumps_[static_cast<size_t>(section)].view();
}

bool Lump::previous_version()
{
    assert(m_context);
    m_stream = nullptr;
    if (m_context->m_previous) {
        m_context = m_context->m_previous.get();
        return true;
    }
    return false;
}

bool Lump::has_previous_version() const
{
    assert(m_context);
    return m_context->m_previous != nullptr;
}

bool Lump::next_version()
{
    assert(m_context);
    m_stream = nullptr;
    if (m_context->m_next) {
        m_context = m_context->m_next;
        return true;
    }
    return false;
}

bool Lump::has_next_version() const
{
    assert(m_context);
    return m_context->m_next != nullptr;
}

bool Lump::first_version()
{
    auto old = m_context;
    while (previous_version());
    return old != m_context;
}

bool Lump::last_version()
{
    auto old = m_context;
    while (next_version());
    return old != m_context;
}
