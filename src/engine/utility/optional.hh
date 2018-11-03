// -*- mode: c++ -*-
#ifndef __IMP_OPTIONAL__85951861
#define __IMP_OPTIONAL__85951861

#include <optional>

namespace imp {
  template <class T>
  using Optional = std::optional<T>;

  using nullopt_t = std::nullopt_t;
  using std::nullopt;

  template <class T, class... Args>
  inline Optional<T> make_optional(Args&&... args)
  {
      return { T(std::forward<Args>(args)...) };
  }
}

#endif //__IMP_OPTIONAL__85951861
