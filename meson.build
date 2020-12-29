project('uzu', 'c',
  version : '1.0',
  default_options : ['warning_level=3', 'c_std=gnu11'])

subdir('src')

project_name = 'uzu'
incdirs = include_directories('src')
deps = [
  dependency('SDL2', required: true), 
  dependency('SDL2_image', required: true), 
  dependency('SDL2_mixer', required: true), 
  dependency('cglm'),
  dependency('SDL2_gfx'),
  dependency('SDL2_ttf'),
  dependency('json-c', static: true),
]

executable(project_name, source_files, 
  include_directories: incdirs,
  dependencies: deps,
  install : true,
  install_dir: meson.source_root() / 'dist'
)

install_subdir('asserts', install_dir:  meson.source_root() / 'dist')
