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

add_test(
  NAME readbst_test
  COMMAND bash -c "${CMAKE_CURRENT_SOURCE_DIR}/scripts/test-run.rb -t ${CMAKE_BINARY_DIR}/apps/readbst -i ${CMAKE_CURRENT_SOURCE_DIR}/readbst/001.dat -e ${CMAKE_CURRENT_SOURCE_DIR}/readbst/001.ans"
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set_tests_properties(readbst_test PROPERTIES DEPENDS readbst)