cmake_minimum_required(VERSION 3.20)
project(boketto)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_subdirectory(src/core)
add_subdirectory(src/core/arm)
add_subdirectory(src/core/arm/instruction)
add_subdirectory(src/core/cpu)
add_subdirectory(src/core/io)
add_subdirectory(src/core/scheduler)
add_subdirectory(src/core/thumb)
add_subdirectory(src/core/thumb/instruction)
add_subdirectory(external/nativefiledialog-extended)
add_subdirectory(external/cimgui)

find_package(OpenGL REQUIRED)
find_package(capstone CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)

add_executable(boketto
  src/main.c src/core.c src/disasm.c src/gui.c src/util/bit.c
)

if (CMAKE_BUILD_TYPE MATCHES Debug)
  target_compile_options(boketto PUBLIC -g)
  #target_compile_options(boketto PUBLIC -fsanitize=address)
  #target_link_options(boketto PUBLIC -fsanitize=address)
elseif(CMAKE_BUILD_TYPE MATCHES Release)
  target_compile_options(boketto PUBLIC -O3)
endif()

target_include_directories(boketto PUBLIC
  src
  src/util
  src/core
  src/core/cpu
  src/core/io
  src/core/scheduler
  external
  external/nativefiledialog-extended
  external/cimgui
  external/cimgui/imgui
  external/cimgui/generator/output)
target_link_libraries(boketto PUBLIC nfd cimgui capstone core arm arm-instructions cpu io scheduler thumb thumb-instructions)
