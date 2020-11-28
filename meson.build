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

ecs_test_name = 'ecs_test'
ecs_test_deps = deps + [check_dep]
ecs_test_incdirs = include_directories('src')
ecs_source_files = ['test/ecs_pool_test.c', 'src/ecs/pool.c']

ecs_test_exe = executable(ecs_test_name, ecs_source_files,
  include_directories: ecs_test_incdirs,
  dependencies: ecs_test_deps,
  install: false
)

