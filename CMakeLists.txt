cmake_minimum_required(VERSION 3.20)
project(boketto)

set(BUILD_SHARED_LIBS OFF)
set(CMAKE_C_STANDARD 11)

find_package(SDL2 CONFIG REQUIRED)

include_directories(
  src
  src/util
  src/frontend
  src/backend
  src/backend/core
  src/backend/core/arm
  src/backend/core/thumb
  src/backend/core/cpu
  src/backend/core/io
  src/backend/core/scheduler
  external/nativefiledialog-extended/src/include
  ${SDL2_INCLUDE_DIRS}
)

if (WIN32)
  add_compile_definitions(NOMINMAX)
  add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif (WIN32)
add_compile_definitions(SDL_MAIN_HANDLED)

add_subdirectory(src/frontend)
add_subdirectory(src/backend)
add_subdirectory(src/backend/core)
add_subdirectory(src/backend/core/arm)
add_subdirectory(src/backend/core/cpu)
add_subdirectory(src/backend/core/io)
add_subdirectory(src/backend/core/scheduler)
add_subdirectory(src/backend/core/thumb)
add_subdirectory(external/nativefiledialog-extended)

add_executable(boketto main.c)
file(COPY ${PROJECT_SOURCE_DIR}/resources/ DESTINATION ${PROJECT_BINARY_DIR}/resources/)

target_link_libraries(boketto PUBLIC SDL2::SDL2main SDL2::SDL2 nfd frontend backend core arm cpu io scheduler thumb)
