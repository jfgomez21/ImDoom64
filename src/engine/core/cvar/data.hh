#ifndef __DATA__34737289
#define __DATA__34737289

#include <prelude.hh>
#include <functional>
#include <charconv>
#include <string>

#include <variant>

#include "flags.hh"

namespace imp::cvar::detail {
  struct cast_to_string {
      bool operator()(std::string& val, std::string_view new_val) const
      {
          val = new_val;
          return true;
      }

      bool operator()(bool& val, std::string_view new_val) const
      {
          if (new_val == "true" || new_val == "yes" || new_val == "on") {
              val = true;
              return true;
          }

          if (new_val == "false" || new_val == "no" || new_val == "off") {
              val = false;
              return  true;
          }

          /* fallback to int conversion */
          int intval {};
          if ((*this)(intval, new_val)) {
              val = (intval != 0);
              return true;
          }

          return false;
      }

      template <class T>
      bool operator()(T& val, std::string_view new_val) const
      {
          /* Will enable in the future when better C++17 support is a thing */
#if 0
          auto conv_res = std::from_chars(new_val.data(), new_val.data() + new_val.size(), val);

          /* check for conversion errors */
          if (conv_res.ec != std::errc{}) {
              /* Don't change value and ignore */
              DEBUG("Can't cast '{}' to {}", new_val, typeid(T).name());
              return false;
          }

          return true;
#endif

          try {
              if constexpr (std::is_same_v<T, int>) {
                  val = std::stoi(std::string {new_val});
              } else if constexpr (std::is_same_v<T, float>) {
                  val = std::stof(std::string {new_val});
              }

              return true;
          } catch (std::logic_error& e) {
              /* Don't change value and ignore */
              DEBUG("Can't cast '{}' to {}", new_val, typeid(T).name());
              return false;
          }
      }
  };
}

namespace imp::cvar {
  template <class T>
  using Callback = std::function<void (const T&)>;

  template <class T>
  using OptCallback = Optional<Callback<T>>;

  template <class T>
  using VTuple = std::tuple<T, T, OptCallback<T>>;

  using Variant = std::variant<
      VTuple<bool>,
      VTuple<int>,
      VTuple<float>,
      VTuple<String>
      >;

  class Data {
      String m_name {};
      String m_desc {};

      bool m_valid { true };

      Variant m_data;

      FlagSet m_flags;

      template <class T>
      VTuple<T>& m_tuple()
      { return std::get<VTuple<T>>(m_data); }

      template <class T>
      const VTuple<T>& m_tuple() const
      { return std::get<VTuple<T>>(m_data); }

  public:
      explicit Data(int def):
          m_data(VTuple<int> { def, def, nullopt }) {}

      explicit Data(float def):
          m_data(VTuple<float> { def, def, nullopt }) {}

      explicit Data(const String& def):
          m_data(VTuple<String> { def, def, nullopt }) {}

      explicit Data(bool def):
          m_data(VTuple<bool> { def, def, nullopt }) {}

      template <class T>
      T& get()
      { return std::get<0>(m_tuple<T>()); }

      template <class T>
      const T& get() const
      { return std::get<0>(m_tuple<T>()); }

      template <class T>
      const T& get_default() const
      { return std::get<1>(m_tuple<T>()); }

      bool is_valid() const
      { return m_valid; }

      StringView name() const
      { return m_name; }

      void set_name(StringView name)
      { m_name = name; }

      StringView desc() const
      { return m_desc; }

      void set_desc(StringView desc)
      { m_desc = desc; }

      void set_valid(bool valid)
      { m_valid = valid; }

      void set_flags(FlagSet flags)
      { m_flags = flags; }

      void set_flag(Flag flag)
      { m_flags.set(flag); }

      bool test_flag(Flag flag)
      { return m_flags.test(flag); }

      template <class T>
      void set_callback(const Callback<T>& cb)
      {
          std::get<2>(m_tuple<T>()) = cb;
      }

      void set_to_default()
      {
          std::visit([](auto& var) {
              std::get<0>(var) = std::get<1>(var);
          }, m_data);
      }

      void set_value(StringView new_value)
      {
          std::visit([&new_value](auto& var) {
              detail::cast_to_string {}(std::get<0>(var), new_value);
          }, m_data);
      }

      String get_value() const
      {
          return std::visit([](const auto& var) {
              using type = std::decay_t<decltype(std::get<0>(var))>;
              if constexpr (std::is_same_v<type, std::string>) {
                  return std::get<0>(var);
              } else {
                  return std::to_string(std::get<0>(var));
              }
          }, m_data);
      }

      String get_default_string() const
      {
          return std::visit([](const auto& var) {
              using type = std::decay_t<decltype(std::get<1>(var))>;
              if constexpr (std::is_same_v<type, std::string>) {
                  return std::get<1>(var);
              } else {
                  return std::to_string(std::get<1>(var));
              }
          }, m_data);
      }

      void update() const
      {
          std::visit([](const auto &var) {
              auto ocb = std::get<2>(var);
              if (ocb) {
                  (*ocb)(std::get<0>(var));
              }
          }, m_data);
      }

      const FlagSet& flags() const
      { return m_flags; }
  };
}

#endif //__DATA__34737289
