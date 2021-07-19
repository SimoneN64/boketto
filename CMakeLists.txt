cmake_minimum_required(VERSION 3.7.0)
project(boketto)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

file(GLOB_RECURSE EMU_SRC "src/*.c")

include_directories(
  ${CMAKE_SOURCE_DIR}/include/
  ${CMAKE_SOURCE_DIR}/include/util/
  ${CMAKE_SOURCE_DIR}/include/core/
  ${CMAKE_SOURCE_DIR}/include/core/cpu/
  ${CMAKE_SOURCE_DIR}/include/core/arm/instruction/
  ${CMAKE_SOURCE_DIR}/include/external/
  ${CMAKE_SOURCE_DIR}/include/external/nativefiledialog-extended/
)

if(WIN32)
  set(FLAGS -static SDL2main SDL2 gcc stdc++ winpthread winmm version Imm32 Setupapi)
else()
  set(FLAGS SDL2main SDL2)
endif()

find_package(SDL2 REQUIRED)
add_subdirectory(include/external/nativefiledialog-extended/)

add_compile_options(-g)
add_compile_definitions(DEBUG)

#add_link_options(-fsanitize=address)

add_executable(${CMAKE_PROJECT_NAME}
  ${IMGUI_SRC}
  ${IMPLOT_SRC}
  ${EMU_SRC}
)

target_link_libraries(${CMAKE_PROJECT_NAME} ${FLAGS} nfd)