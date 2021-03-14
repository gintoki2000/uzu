project('uzu', 'c',
  version : '1.0',
  default_options : ['warning_level=3', 'c_std=c11'])

subdir('src')

project_name = 'uzu'
incdirs = include_directories('include')
deps = [
  dependency('SDL2', required: true), 
  dependency('SDL2_image', required: true), 
  dependency('SDL2_mixer', required: true), 
  dependency('SDL2_gfx', required: true),
  dependency('SDL2_ttf', required: true),
  dependency('json-c', static: true, required: true),
]

dist_dir = meson.source_root() / 'dist'

executable(project_name, source_files, 
  include_directories: incdirs,
  dependencies: deps,
  install : true,
  install_dir: dist_dir
)

install_subdir('res', install_dir: dist_dir)
