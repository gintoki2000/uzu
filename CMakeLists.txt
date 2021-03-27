cmake_minimum_required(VERSION 3.1)
#add find sdl2 and sdl2 image modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_C_FLAGS "-Wall -Wextra -Wpedantic")
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
project(dungeon LANGUAGES C)

include(src/CMakeLists.txt)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
		find_package(SDL2)
		find_package(SDL2_image)
		find_package(SDL2_mixer)
		find_package(SDL2_ttf)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
		set(SDL2_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/libraries/)
		set(SDL2_LIBRARIES ${CMAKE_SOURCE_DIR}/libraries/)

		set(SDL2_IMAGE_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/libraries/)
		set(SDL2_IMAGE_LIBRARIES ${CMAKE_SOURCE_DIR}/libraries/)

		set(SDL2_MIXER_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/libraries/)
		set(SDL2_MIXER_LIBRARIES ${CMAKE_SOURCE_DIR}/libraries/)

		set(SDL2_TTF_LIBRARIES ${CMAKE_SOURCE_DIR}/libraries/)
		set(SDL2_TTF_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/libraries/)

		set(JSON_C_LIBS ${CMAKE_SOURCE_DIR}/libraries/)
		set(JSON_C_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/libraries/)
endif()

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE 
		${SDL2_INCLUDE_DIRS} 
		${SDL2_IMAGE_INCLUDE_DIRS} 
		${SDL2_MIXER_INCLUDE_DIRS} 
		${SDL2_TTF_INCLUDE_DIRS}
		${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(${PROJECT_NAME} PRIVATE 
		${SDL2_LIBRARIES} 
		${SDL2_IMAGE_LIBRARIES} 
		${SDL2_MIXER_LIBRARIES} 
		${SDL2_TTF_LIBRARIES}
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:${PROJECT_NAME}>/res
)
