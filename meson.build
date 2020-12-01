project('uzu', 'c',
  version : '1.0',
  default_options : ['warning_level=3', 'c_std=c11'])

subdir('src')

project_name = 'uzu'
incdirs = include_directories('src')
deps = [dependency('SDL2', required: true), dependency('SDL2_image', required: true)]

executable(project_name, source_files, 
  include_directories: incdirs,
  dependencies: deps,
  install : true,
  install_dir: meson.source_root() / 'dist'
)

install_subdir('asserts', install_dir:  meson.source_root() / 'dist')

check_dep = dependency('check')

executable('rect_test', toolbox_source_files + files('test/rect_test.c'), 
  dependencies: deps + [check_dep], 
  install: false, 
  include_directories: incdirs
)
