#------------------------------------------------------------------------------
#
# Top-level C++20 build for the header libraries, CLI applications, tests,
# sanitizers, warnings, and optional LLVM coverage instrumentation.
#
#------------------------------------------------------------------------------
#
# This file is licensed after LGPL v3
# Look at: https://www.gnu.org/licenses/lgpl-3.0.en.html for details
#
#------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.20)
project(computability LANGUAGES CXX)

include(CTest)

option(ENABLE_WARNINGS "Enable common compiler warnings" ON)
if(ENABLE_WARNINGS)
  if(MSVC)
    add_compile_options(/W4)
  elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    add_compile_options(-Wall -Wextra -Wpedantic)
  endif()
endif()

option(ENABLE_SANITIZERS "Enable AddressSanitizer and UndefinedBehaviorSanitizer" OFF)
if(ENABLE_SANITIZERS)
  if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    message(FATAL_ERROR "ENABLE_SANITIZERS requires Clang or GCC")
  endif()
  add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
  add_link_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
endif()

# coverage
option(ENABLE_COVERAGE "Enable Clang/LLVM source-based coverage" OFF)
include(cmake/Coverage.cmake)
computability_enable_coverage_if_requested()

# framework
add_library(frame INTERFACE)
target_include_directories(frame INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/framework)

# header-only library
add_library(comblib INTERFACE)
target_include_directories(comblib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/combinatorial)

# applications
add_subdirectory(apps)

# tests
add_subdirectory(tests)

computability_add_coverage_targets_if_requested()
