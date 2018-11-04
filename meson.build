##------------------------------------------------------------------------------
##
## ImDoom 64 meson configuration
##
## LICENSE:
##
## Copyright(C) 2018 Zohar Malamant
##
## This program is free software; you can redistribute it and/or
## modify it under the terms of the GNU General Public License
## as published by the Free Software Foundation; either version 2
## of the License, or (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
## 02111-1307, USA.
##

project(
  'ImDoom64', ['c', 'cpp'],
  version : '0.1.0',
  meson_version : '>=0.46.0',
  license : 'GPLv2',
  default_options : [
    'cpp_std=c++17'
  ]
)

cpp = meson.get_compiler('cpp')

cpp_args = []
if get_option('buildtype') == 'debug'
  cpp_args = ['-DIMP_DEBUG']
endif

##------------------------------------------------------------------------------
## Options
##
program_name = get_option('program_name')
enable_gtk3 = target_machine.system() == 'linux' and get_option('enable_gtk3')

if enable_gtk3
  add_languages('vala')
endif

##------------------------------------------------------------------------------
## Dependencies
##

# Threads
threads_dep = dependency('threads')

# SDL2 & co.
sdl2_dep = dependency('sdl2')
sdl2_net_dep = dependency('SDL2_net')

# zlib & libpng
zlib_dep = dependency('zlib')
png_dep = dependency('libpng')

# fmtlib
fmt_dep = dependency('fmt', version : ['>=5.2.1'], fallback : ['fmt', 'fmt_dep'])

# GLBinding
glbinding_dep = []
if get_option('enable_glbinding')
  glbinding_dep = dependency(
    'glbinding',
    version : '3.0.2',
    fallback : ['glbinding', 'glbinding_dep']
  )
endif

# Fluidsynth
fluid_dep = dependency('fluidsynth', fallback : ['fluidsynth-lite', 'fluidsynth_dep'])

##------------------------------------------------------------------------------
## Target: imdoom64.pk3 (data)
##
zip_prog = find_program('zip', required : false, native : true)
if zip_prog.found()
  zip_src = files(
    'data/imdoom64.pk3/animdefs.txt',
    'data/imdoom64.pk3/mapinfo.txt',
    'data/imdoom64.pk3/palplay3.act',
    'data/imdoom64.pk3/skydefs.txt',
    'data/imdoom64.pk3/graphics/buttons.png',
    'data/imdoom64.pk3/graphics/confont.png',
    'data/imdoom64.pk3/graphics/crshairs.png',
    'data/imdoom64.pk3/graphics/cursor.png',
    'data/imdoom64.pk3/graphics/fancred.png',
  )

  if target_machine.system() == 'linux'
    install_dir = 'share/imdoom64'
  elif target_machine.system() == 'darwin'
    install_dir = 'Resource/ImDoom64'
  else
    error('Unsupported target OS!')
  endif

  pk3_target = custom_target(
    'pk3',
    output : 'imdoom64.pk3',
    input : zip_src,
    command : [join_paths(meson.current_source_dir(), 'script/meson_build_pk3.sh'), meson.current_source_dir(), meson.current_build_dir()],
    install : true,
    install_dir : install_dir,
    build_by_default : true)
else
  warning('`zip` program not found. Can\'t generate `imdoom64.pk3`')
endif

##------------------------------------------------------------------------------
## Target: Native UI - Gtk3 (gtk3)
##
if enable_gtk3
  subdir('src/gtk3')
else
  nui_gtk3_dep = []
endif

##------------------------------------------------------------------------------
## Target: imdoom64 (engine)
##
subdir('src/engine')

executable(
  program_name,
  sources : engine_src,
  dependencies : engine_dep,
  include_directories : engine_inc,
  cpp_args : cpp_args,
  install : true
)

##------------------------------------------------------------------------------
## Install: Linux misc data
##
if target_machine.system() == 'linux'
  icon_sizes = ['16x16', '22x22', '24x24', '32x32', '64x64', '128x128', '256x256']
  data_dir = join_paths(get_option('prefix'), 'share')

  foreach icon_size : icon_sizes
    install_data(
      join_paths('data/icons', icon_size, 'imdoom64.png'),
      install_dir : join_paths(data_dir, 'icons/hicolor', icon_size, 'apps'),
      rename : '@0@.png'.format(program_name)
    )
  endforeach

  desktop_data = configuration_data()
  desktop_data.set('program_name', program_name)

  ##
  # freedesktop Desktop file
  configure_file(
    input : 'data/imdoom64.desktop.in',
    output : '@0@.desktop'.format(program_name),
    configuration : desktop_data
  )
  install_data(
    join_paths(meson.current_build_dir(), '@0@.desktop'.format(program_name)),
    install_dir : join_paths(data_dir, 'applications')
  )

  ##
  # freedesktop Appstream file
  configure_file(
    input : 'data/imdoom64.appdata.xml.in',
    output : '@0@.appdata.xml'.format(program_name),
    configuration : desktop_data
  )
  install_data(
    join_paths(meson.current_build_dir(), '@0@.appdata.xml'.format(program_name)),
    install_dir : join_paths(data_dir, 'metainfo')
  )

  # install_data(
  #   'data/imdoom64.6',
  #   install_dir : 'usr/share/man'
  # )
endif

##------------------------------------------------------------------------------
## Install: MacOS misc data
##
if target_machine.system() == 'darwin'
  install_data('data/ImDoom64.icns', install_dir : 'Contents/Resources')
  install_data('data/Info.plist', install_dir : 'Contents')
endif

##------------------------------------------------------------------------------
## Postinstall
##
if target_machine.system() == 'linux'
  meson.add_install_script(
    'script/post_install.py',
    data_dir
  )
elif target_machine.system() == 'darwin'
  meson.add_install_script('script/macos_install_script.sh')
endif
