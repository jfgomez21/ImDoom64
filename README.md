ImDoom 64 [![Build Status](https://travis-ci.org/Doom64/ImDoom64.svg?branch=master)](https://travis-ci.org/Doom64/ImDoom64) [![Build status](https://ci.appveyor.com/api/projects/status/s2506ar0a2y4px0s?svg=true)](https://ci.appveyor.com/project/dotfloat/imdoom64)
========

ImDoom 64 is a reverse-engineering project aimed to recreate Doom 64 as close as
possible with additional modding features. This project is a fork of
[Doom64EX](https://github.com/svkaiser/Doom64EX).

# Usage

Unlike Doom64EX, ImDoom 64 uses an original, unmodified N64 ROM. As a result,
there is no `Wadgen` utility. To use, rename your legally obtained ROM to
`doom64.rom` and place it in one of ImDoom 64's [search paths](#search-paths).

# Compiling

## Linux

Dependencies:

- `git`
- C++17-compliant compiler (`gcc` 8).
- [Meson](https://mesonbuild.com/Getting-meson.html) 0.47 or newer
- [ninja](https://ninja-build.org/) 1.5 or newer.
- `SDL2`
- `SDL2_net`
- `libpng` 1.6 or newer
- `zlib`
- `fluidsynth` (optional)
- [`fmt`](http://fmtlib.net/latest/index.html) 5.2.1 or newer (optional)

The optional dependencies will be downloaded and compiled automatically by
`meson` if missing.

Once everything is installed, do:

```bash
$ # Get the sources
$ git clone https://github.com/Doom64/ImDoom64.git --recursive

$ # Enter build directory
$ mkdir -p ImDoom64/build
$ cd ImDoom64/build

$ # Build
$ meson --default-library=static --buildtype=release ..
$ ninja
$ sudo ninja install
```

ImDoom 64 is now usable using the `imdoom64` command or through the `ImDoom 64`
icon in your system menus.

# Search Paths

## Linux

The game will look for files in your `$XDG_DATA_DIR/imdoom64` directory. This is
normally `~/.local/share/imdoom64`.

Additionally, it will look in the following system directories:

- `/usr/local/share/games/imdoom64`
- `/usr/local/share/imdoom64`
- `/usr/local/share/doom`
- `/usr/share/games/imdoom64`
- `/usr/share/imdoom64`
- `/usr/share/doom`
- `/opt/imdoom64`
- `/app/imdoom64`

## Linux Flatpak

When running the flatpak version, the program's filesystem access is limited.
The only directory that is accessible is
`~/.var/app/com.dotfloat.ImDoom64/data/imdoom64`.

## Windows

The game will look for files in the same location the `Doom64.exe`.

# Community

**[Discord](https://discord.gg/AHd8t33)**
