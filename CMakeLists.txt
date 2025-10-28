cmake_minimum_required(VERSION 3.20)
project(sdl3_hello CXX)

# --- Global language/version
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# =========================
# SDL3 discovery (once)
# =========================
# On Apple, default to using the .xcframework you dropped into ~/Library/Frameworks.
# You can override SDL3_XC_DIR or SDL3_USE_XCFRAMEWORK at configure time.
option(SDL3_USE_XCFRAMEWORK "Use the SDL3.xcframework on Apple platforms" ON)

set(SDL3_XC_DIR "$ENV{HOME}/Library/Frameworks/SDL3.xcframework"
    CACHE PATH "Path to SDL3.xcframework (Apple only)")

function(_ensure_sdl3_target)
  if(TARGET SDL3::SDL)
    return() # already defined
  endif()

  if(APPLE AND SDL3_USE_XCFRAMEWORK)
    # ----- Pick a slice from the xcframework
    set(_candidates
      "${SDL3_XC_DIR}/macos-arm64_x86_64"
      "${SDL3_XC_DIR}/macos-arm64"
      "${SDL3_XC_DIR}/macos-x86_64"
    )
    set(_SDL3_SLICE "")
    foreach(dir IN LISTS _candidates)
      if(EXISTS "${dir}/SDL3.framework")
        set(_SDL3_SLICE "${dir}")
        break()
      endif()
    endforeach()

    if(NOT _SDL3_SLICE)
      message(FATAL_ERROR "Couldn't find SDL3.framework slice inside: ${SDL3_XC_DIR}")
    endif()

    # ----- Create an imported interface for consistent linking
    add_library(SDL3::SDL INTERFACE IMPORTED)
    target_include_directories(SDL3::SDL INTERFACE "${_SDL3_SLICE}/SDL3.framework/Headers")
    # Use the actual .framework as a link item, and ensure -F is set
    target_link_libraries(SDL3::SDL INTERFACE "${_SDL3_SLICE}/SDL3.framework")
    target_link_directories(SDL3::SDL INTERFACE "${_SDL3_SLICE}")
    # Let CMake know where to find frameworks when generating buildsystem
    set(CMAKE_FRAMEWORK_PATH
        "${_SDL3_SLICE};${CMAKE_FRAMEWORK_PATH}"
        CACHE STRING "Framework search paths" FORCE)

  else()
    # ----- Non-Apple (or Apple without xcframework): try config package first
    find_package(SDL3 CONFIG QUIET)
    if(SDL3_FOUND)
      # The official config exports targets like SDL3::SDL3 (shared) or SDL3::SDL3-static.
      if(TARGET SDL3::SDL3)
        add_library(SDL3::SDL INTERFACE IMPORTED)
        target_link_libraries(SDL3::SDL INTERFACE SDL3::SDL3)
      elseif(TARGET SDL3::SDL3-static)
        add_library(SDL3::SDL INTERFACE IMPORTED)
        target_link_libraries(SDL3::SDL INTERFACE SDL3::SDL3-static)
      else()
        message(FATAL_ERROR "SDL3 config found, but expected targets not present.")
      endif()
    else()
      # ----- Fallback: pkg-config
      find_package(PkgConfig REQUIRED)
      pkg_check_modules(SDL3 REQUIRED sdl3)

      add_library(SDL3::SDL INTERFACE IMPORTED)
      target_include_directories(SDL3::SDL INTERFACE ${SDL3_INCLUDE_DIRS})
      target_link_libraries(SDL3::SDL INTERFACE ${SDL3_LINK_LIBRARIES})
      target_compile_options(SDL3::SDL INTERFACE ${SDL3_CFLAGS_OTHER})
      # Link directories only if provided (older pkg-config setups)
      if(SDL3_LIBRARY_DIRS)
        target_link_directories(SDL3::SDL INTERFACE ${SDL3_LIBRARY_DIRS})
      endif()
    endif()
  endif()
endfunction()

# Ensure the interface target exists
_ensure_sdl3_target()

# =========================
# Helper to add an SDL app
# =========================
function(add_sdl3_app name)
  if(ARGC LESS 2)
    message(FATAL_ERROR "add_sdl3_app(<name> <source1> [source2 ...]) requires at least one source")
  endif()
  add_executable(${name} ${ARGN})
  target_link_libraries(${name} PRIVATE SDL3::SDL)
  target_compile_features(${name} PRIVATE cxx_std_17)

  # Nice defaults for warnings (tweak to taste)
  if(MSVC)
    target_compile_options(${name} PRIVATE /W4 /permissive-)
  else()
    target_compile_options(${name} PRIVATE -Wall -Wextra -Wpedantic)
  endif()
endfunction()


add_sdl3_app(sdl_keypad        sdl3/keypad.cpp)
add_sdl3_app(sdl_checkerboard  sdl3/checkerboard.cpp)


set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_target_properties(sdl_keypad       PROPERTIES FOLDER "apps")
set_target_properties(sdl_checkerboard PROPERTIES FOLDER "apps")
