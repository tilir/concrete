#------------------------------------------------------------------------------
#
# CMake build system for permloops app
# input: permutation, output: permutation loops
# like 1 7 10 4 5 6 9 8 2 3 -> (1)(2 7 9)(3 10)(4)(5)(6)(8)
#
#------------------------------------------------------------------------------
#
# This file is licensed after LGPL v3
# Look at: https://www.gnu.org/licenses/lgpl-3.0.en.html for details
#
#------------------------------------------------------------------------------

add_test(
  NAME permloops_test
  COMMAND bash -c "${TEST_SCRIPT} -t ${permloops_app} -f ${CMAKE_CURRENT_SOURCE_DIR}"
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set_tests_properties(permloops_test PROPERTIES DEPENDS permloops)
