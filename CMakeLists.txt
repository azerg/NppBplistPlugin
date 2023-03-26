cmake_minimum_required(VERSION 3.20)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(CMAKE_CONFIGURATION_TYPES "Debug;RelWithDebInfo" CACHE STRING "" FORCE)

project(NppBplistPlugin VERSION 3.0)

if(MSVC)
  add_definitions(-DUNICODE -D_UNICODE)
endif()

# Set the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include vcpkg packages
set(VCPKG_LIBRARY_LINKAGE static)
set(VCPKG_CRT_LINKAGE static)

add_subdirectory(src/NppBplistPlugin/src)