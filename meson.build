project('uzu', 'c',
  version : '1.0',
  default_options : ['warning_level=3', 'c_std=c11'])

subdir('src')

PROJECT_NAME = 'uzu'
INCLUDE_DIRECTORIES = include_directories('src')
DEPENDENCIES = [dependency('SDL2', required: true), dependency('SDL2_image', required: true)]

UZU = executable(PROJECT_NAME, SOURCE_FILES, 
  include_directories: INCLUDE_DIRECTORIES,
  dependencies: DEPENDENCIES,
  install : false
)

