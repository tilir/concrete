#------------------------------------------------------------------------------
#
# CMake build system for computability experiments
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

# coverage
option(ENABLE_COVERAGE "Enable Clang/LLVM source-based coverage" OFF)
include(cmake/Coverage.cmake)
computability_enable_coverage_if_requested()

# framework
add_library(frame INTERFACE)
target_include_directories(frame INTERFACE framework)

# header-only library
add_library(comblib INTERFACE)
target_include_directories(comblib INTERFACE combinatorial)

# applications
add_subdirectory(apps)

# tests
add_subdirectory(tests)

computability_add_coverage_targets_if_requested()

install()

