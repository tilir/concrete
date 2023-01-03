#------------------------------------------------------------------------------
#
# CMake build system for readbst app
# test for reading permutation and output of edge list
#
#------------------------------------------------------------------------------
#
# This file is licensed after LGPL v3
# Look at: https://www.gnu.org/licenses/lgpl-3.0.en.html for details
#
#------------------------------------------------------------------------------

add_test(
  NAME readbst_test
  COMMAND bash -c "${TEST_SCRIPT} -t ${readbst_app} -f ${CMAKE_CURRENT_SOURCE_DIR}"
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set_tests_properties(readbst_test PROPERTIES DEPENDS readbst)