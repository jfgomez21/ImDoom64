#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif

#ifdef __linux__
#include <sys/sendfile.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif

#include <platform/app.hh>
#include "core/args.hh"
#include "native_ui/native_ui.hh"
#include "wad.hh"
#include "wad_loaders.hh"

extern std::filesystem::path data_dir;

void wad::init()
{
    Optional<String> path;
    bool iwad_loaded {};

    // Add device loaders
    wad::add_device_loader(zip_loader);
    wad::add_device_loader(doom_loader);
    wad::add_device_loader(rom_loader);

    /* Find and add the Doom 64 IWAD */
    while (!iwad_loaded) {
        if ((path = app::find_data_file("doom64.rom"))) {
            iwad_loaded = wad::add_device(*path);
        }

        if (!iwad_loaded) {
            auto path = g_native_ui->rom_select();

            if (!path) {
                log::fatal("Couldn't find 'doom64.rom'");
            }

            std::filesystem::copy_file(*path, data_dir / "doom64.rom"sv);
        }
    }

    // Find and add 'imdoom64.pk3'
    if (auto engine_data_path = app::find_data_file("imdoom64.pk3")) {
        wad::add_device(*engine_data_path);
    } else {
        log::fatal("Couldn't find 'imdoom64.pk3'");
    }

    // Add any additional WADs
    bool add {};
    for (const auto& tok : args::all_args()) {
        if (add && tok[0] != '-') {
            log::info("Adding WAD '{}'", tok);
            iwad_loaded = wad::add_device(tok);
            if (!iwad_loaded) {
                log::fatal("Failed inserting '{}'", tok);
            }
        } else if (add && tok[0] == '-') {
            break;
        } else if (tok == "-file") {
            add = true;
        }
    }

    wad::merge();
}
