#------------------------------------------------------------------------------
#
# CMake build system for invperm app
# input: permutation, output: inverse permutation
# like 1 9 10 4 5 6 2 8 7 3 -> 1 7 10 4 5 6 9 8 2 3
#
#------------------------------------------------------------------------------
#
# This file is licensed after LGPL v3
# Look at: https://www.gnu.org/licenses/lgpl-3.0.en.html for details
#
#------------------------------------------------------------------------------

add_test(
  NAME invperm_test
  COMMAND bash -c "${TEST_SCRIPT} -t ${invperm_app} -f ${CMAKE_CURRENT_SOURCE_DIR}"
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set_tests_properties(invperm_test PROPERTIES DEPENDS invperm)