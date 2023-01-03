#------------------------------------------------------------------------------
#
# CMake build system for readbst app
# tests for braced input, like:
# 4 (()(())) to { 4 -- 2 [left], 2 -- 3 [right], 3 -- 1 [left] }
#
#------------------------------------------------------------------------------
#
# This file is licensed after LGPL v3
# Look at: https://www.gnu.org/licenses/lgpl-3.0.en.html for details
#
#------------------------------------------------------------------------------

add_test(
  NAME readbst_brac_test
  COMMAND bash -c "${TEST_SCRIPT} -t ${readbst_app} -f ${CMAKE_CURRENT_SOURCE_DIR} -a -ibrac"
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set_tests_properties(readbst_brac_test PROPERTIES DEPENDS readbst)
