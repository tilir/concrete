#------------------------------------------------------------------------------
#
# CMake build system for computability experiments
#
# Try:
# > cmake -G Ninja -B build -DCMAKE_BUILD_TYPE=Release .
# > cmake --build build
#
#------------------------------------------------------------------------------
#
# Macros to control things
#
# -DVISUAL=1 -- include apps with SDL2 visualization
#
#------------------------------------------------------------------------------
#
# This file is licensed after LGPL v3
# Look at: https://www.gnu.org/licenses/lgpl-3.0.en.html for details
#
#------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.15)
project(computability)
enable_testing()

add_library(frame INTERFACE)
target_include_directories(frame INTERFACE framework)

add_library(treelib INTERFACE)
target_include_directories(treelib INTERFACE trees)

if(VISUAL)
# library for drawing
add_subdirectory(drawer)
endif()

# applications
add_subdirectory(apps)

add_subdirectory(tests)

install()