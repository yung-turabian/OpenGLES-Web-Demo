cmake_minimum_required(VERSION 3.10)

option(PRODUCTION_BUILD "Make this a production build" OFF)
option(MYGAME_VENDORED "Use vendored libraries" OFF)

project(SokobanGameDemo VERSION 0.0.1 LANGUAGES CXX; C)

# Set compiler
if(DEFINED EMSCRIPTEN)
  set(CMAKE_C_COMPILER "/home/henry/emsdk/upstream/emscripten/emcc")
  set(CMAKE_CXX_COMPILER "/home/henry/emsdk/upstream/emscripten/em++")
  set(CMAKE_EXECUTABLE_SUFFIX ".html")
else()
  set(CMAKE_C_COMPILER "clang")
  set(CMAKE_CXX_COMPILER "clang++")
endif()

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
cmake_policy(SET CMP0069 NEW)

if (WIN32)
    # Forcing MSVC to use utf-8 encoding
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif(WIN32)

# Add other external libraries
add_subdirectory(include/glad) # OpenGL loader

# Gather source files
file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# Define the executable
add_executable("${CMAKE_PROJECT_NAME}" ${MY_SOURCES})

# Set target properties and include directories
set_target_properties("${CMAKE_PROJECT_NAME}" PROPERTIES CXX_STANDARD 17)
target_include_directories("${CMAKE_PROJECT_NAME}" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")

if (NOT MSVC)
    target_compile_options(${PROJECT_NAME} PUBLIC "-O2")
endif()

# Include vendored or system SDL libraries
if(MYGAME_VENDORED)
    add_subdirectory(vendored/sdl EXCLUDE_FROM_ALL)
else()
  # 1. Look for a SDL2 package, 2. look for the SDL2 component and 3. fail if none can be found
  find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2 SDL2main)
endif()


# Setup MSVC runtime library and compile definitions
if(MSVC)
	target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC _CRT_SECURE_NO_WARNINGS)
	set_target_properties("${CMAKE_PROJECT_NAME}" PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup") #no console
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>")
endif()

# Set resource paths based on build type
if(PRODUCTION_BUILD)
    target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC RESOURCES_PATH="./res/")
    # remove the option to debug asserts.
	target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC PRODUCTION_BUILD=1) 
else()
    # This is useful to get an ASSETS_PATH in your IDE during development
	target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/res/")
	target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC PRODUCTION_BUILD=0) 
endif()

if(DEFINED EMSCRIPTEN)
  add_compile_definitions("__EMSCRIPTEN__")
  target_link_libraries("${CMAKE_PROJECT_NAME}" PRIVATE glad)
  target_link_options(${CMAKE_PROJECT_NAME} PRIVATE
        "-sDISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=1"
        "-sALLOW_MEMORY_GROWTH=1"
        "-sUSE_SDL=2"
        "-sASYNCIFY=1"
        "-sASSERTIONS=1"
        "-sGL_ASSERTIONS=1"
        "-O2"
        "-sWASM=1"
        "-sFULL_ES3=1"
        "-sMAX_WEBGL_VERSION=2"
        "-sGL_PREINITIALIZED_CONTEXT=1"
        "--preload-file"
        "${CMAKE_CURRENT_SOURCE_DIR}/res@/res"
    )
else()
  # Link libraries
  target_link_libraries("${CMAKE_PROJECT_NAME}" PRIVATE glad SDL2::SDL2)
  if(TARGET SDL2::SDL2main)
      target_link_libraries("${CMAKE_PROJECT_NAME}" PRIVATE SDL2::SDL2main)
  endif()
endif()
