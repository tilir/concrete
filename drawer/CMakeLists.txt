#------------------------------------------------------------------------------
#
# CMake build system for drawer library
#
#------------------------------------------------------------------------------
#
# This file is licensed after LGPL v3
# Look at: https://www.gnu.org/licenses/lgpl-3.0.en.html for details
#
#------------------------------------------------------------------------------

set(SOURCES
  sdlutil.cc
)

add_library(drawer ${SOURCES})
target_compile_features(drawer PUBLIC cxx_std_20)
target_include_directories(drawer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})