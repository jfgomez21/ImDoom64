// -*- mode: c++ -*-
#ifndef __IMP_APP__27387470
#define __IMP_APP__27387470

#include "prelude.hh"
#include "utility/convert.hh"

namespace imp {
  namespace app {
    class Param;

    /** Program entry point */
    [[noreturn]]
    void main(int argc, char** argv);

    bool file_exists(StringView name);

    Optional<String> find_data_file(StringView name, StringView dir_hint = "");

    StringView program();
  }
}

#endif //__IMP_APP__27387470
