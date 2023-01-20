#------------------------------------------------------------------------------
#
# CMake build system for test runs
#
#------------------------------------------------------------------------------
#
# This file is licensed after LGPL v3
# Look at: https://www.gnu.org/licenses/lgpl-3.0.en.html for details
#
#------------------------------------------------------------------------------

set(TEST_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/scripts/test-run.rb")

# basic trees
add_subdirectory(readbst)
add_subdirectory(readbst-back)
add_subdirectory(readbst-topo)
add_subdirectory(readbst-ibrac)
add_subdirectory(readbst-batch)

# permutations
add_subdirectory(permloops)
add_subdirectory(invperm)
add_subdirectory(allperms)
add_subdirectory(baxters)
add_subdirectory(baxters-batch)
add_subdirectory(baxters-filt)

# twin trees
add_subdirectory(readtwin)
add_subdirectory(readtwin-baxters)

# braces
add_subdirectory(allbraces)
