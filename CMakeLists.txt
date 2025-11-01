cmake_minimum_required(VERSION 3.25)
project(chip8-emulator LANGUAGES CXX)

# ---- C++ standard ----
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---- SDL3 detection (prefer CMake config, fallback to pkg-config) ----
set(SDL3_LIB_TARGET "")
find_package(SDL3 QUIET REQUIRED CONFIG COMPONENTS SDL3)
if (SDL3_FOUND)
    set(SDL3_LIB_TARGET SDL3::SDL3)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL3 REQUIRED IMPORTED_TARGET sdl3)
    set(SDL3_LIB_TARGET PkgConfig::SDL3)
endif()

# ---- Include dirs (project root headers like input.hpp, constants.hpp, etc.) ----
# Add more include dirs if you keep headers under include/ or src/
include_directories(${CMAKE_SOURCE_DIR})

# ---- Common compile defs ----
# Only checkerboard.cpp uses the callbacks entrypoints

# =========================
#   Executable: sdl_checkerboard
# =========================
add_executable(sdl_checkerboard
    sdl3/checkerboard.cpp
)
target_link_libraries(sdl_checkerboard PRIVATE ${SDL3_LIB_TARGET})
target_compile_definitions(sdl_checkerboard PRIVATE SDL_MAIN_USE_CALLBACKS=1)
# If your checkerboard includes headers in sdl3/, the include_directories above already covers ${CMAKE_SOURCE_DIR}

# =========================
#   Executable: sdl_keypad
# =========================
add_executable(sdl_keypad
    sdl3/keypad.cpp
)
target_link_libraries(sdl_keypad PRIVATE ${SDL3_LIB_TARGET})

# =========================
#   Executable: chip8 (main + cpu)
# =========================
add_executable(chip8 main.cpp cpu.cpp sdl3/window.cpp)
target_link_libraries(chip8 PRIVATE ${SDL3_LIB_TARGET})

# Put the chip8 runtime **in the repo root** (requested).
# Other targets will go to the build tree as usual.
set_target_properties(chip8 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

# ---- Nice-to-have warnings (clang/gcc) ----
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()
