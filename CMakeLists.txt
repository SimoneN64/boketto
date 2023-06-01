cmake_minimum_required(VERSION 3.7.0)
project(boketto)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_subdirectory(external/nativefiledialog-extended/)
add_subdirectory(external/cimgui/)

target_compile_definitions(cimgui PRIVATE
	IMGUI_IMPL_API=extern\ \"C\")

find_package(OpenGL REQUIRED)
find_package(capstone CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)

include_directories(
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/include/util/
  ${CMAKE_SOURCE_DIR}/include/core/
  ${CMAKE_SOURCE_DIR}/include/core/io/
  ${CMAKE_SOURCE_DIR}/include/core/cpu/
  ${CMAKE_SOURCE_DIR}/include/core/scheduler/
  ${CMAKE_SOURCE_DIR}/external/
  ${CMAKE_SOURCE_DIR}/external/nativefiledialog-extended/
  ${CMAKE_SOURCE_DIR}/external/cimgui/
  ${CMAKE_SOURCE_DIR}/external/cimgui/imgui/
  ${CMAKE_SOURCE_DIR}/external/cimgui/generator/output
)

if (CMAKE_BUILD_TYPE MATCHES Debug)
  add_compile_options(-g)
  #add_compile_options(-fsanitize=address)
  #add_link_options(-fsanitize=address)
elseif(CMAKE_BUILD_TYPE MATCHES Release)
  add_compile_options(-O3)
endif()

add_executable(${CMAKE_PROJECT_NAME}
  ${EMU_SRC}
)

target_link_libraries(${CMAKE_PROJECT_NAME} nfd cimgui capstone)
