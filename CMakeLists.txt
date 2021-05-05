cmake_minimum_required(VERSION 3.1)
#add find sdl2 and sdl2 image modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_C_FLAGS_DEBUG "-g")
set(CMAKE_C_FLAGS_RELEASE "-O3  -fno-strict-aliasing")
set(CMAKE_C_FLAGS "-Wall -Wextra -Wpedantic")
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
project(uzu LANGUAGES C)

include(src/CMakeLists.txt)
find_package(PkgConfig REQUIRED)


if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	find_package(SDL2 REQUIRED)
	find_package(SDL2_image REQUIRED)
	find_package(SDL2_mixer REQUIRED)
	find_package(SDL2_ttf REQUIRED)
	find_package(SDL2_gfx REQUIRED)
		pkg_check_modules(json_c REQUIRED json-c)
		set(JSON_C_LIBRARIES ${json_c_STATIC_LDFLAGS})
		set(JSON_C_INCLUDE_DIRS ${json_c_INCLUDE_DIRS})
		message(STATUS "json-c libraries: ${JSON_C_LIBRARIES}")
		message(STATUS "sdl2 libraries: ${SDL2_LIBRARIES}")
		message(STATUS "sdl2_mixer libraries: ${SDL2_MIXER_LIBRARIES}")
		message(STATUS "sdl2_ttf libraries: ${SDL2_TTF_LIBRARIES}")
		message(STATUS "sdl2_image libraries: ${SDL2_IMAGE_LIBRARIES}")
		message(STATUS "sdl2_gfx libraries: ${SDL2_GFX_LIBRARIES}")
		
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
		set(SDL2_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/libraries/)
		set(SDL2_LIBRARIES ${CMAKE_SOURCE_DIR}/libraries/)

		set(SDL2_IMAGE_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/libraries/)
		set(SDL2_IMAGE_LIBRARIES ${CMAKE_SOURCE_DIR}/libraries/)

		set(SDL2_MIXER_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/libraries/)
		set(SDL2_MIXER_LIBRARIES ${CMAKE_SOURCE_DIR}/libraries/)

		set(SDL2_TTF_LIBRARIES ${CMAKE_SOURCE_DIR}/libraries/)
		set(SDL2_TTF_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/libraries/)

		set(JSON_C_LIBRARIES ${CMAKE_SOURCE_DIR}/libraries/)
		set(JSON_C_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/libraries/)
endif()

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE 
		${SDL2_INCLUDE_DIRS} 
		${SDL2_IMAGE_INCLUDE_DIRS} 
		${SDL2_MIXER_INCLUDE_DIRS} 
		${SDL2_TTF_INCLUDE_DIRS}
		${JSON_C_INCLUDE_DIRS}
		${SDL2_GFX_INCLUDE_DIRS}
		${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(${PROJECT_NAME} PRIVATE 
		${SDL2_LIBRARIES} 
		${SDL2_IMAGE_LIBRARIES} 
		${SDL2_MIXER_LIBRARIES} 
		${SDL2_TTF_LIBRARIES}
		${SDL2_GFX_LIBRARIES}
		${JSON_C_LIBRARIES}
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:${PROJECT_NAME}>/res
)

install(TARGETS ${PROJECT_NAME}
	DESTINATION ${CMAKE_SOURCE_DIR}/dist
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/res DESTINATION ${CMAKE_SOURCE_DIR}/dist)
