#------------------------------------------------------------------------------
#
# CMake build system for readbst app
# tests for batch input, lot of permutations to lot of trees
#
#------------------------------------------------------------------------------
#
# This file is licensed after LGPL v3
# Look at: https://www.gnu.org/licenses/lgpl-3.0.en.html for details
#
#------------------------------------------------------------------------------

add_test(
  NAME readbst_batch_test
  COMMAND bash -c "${TEST_SCRIPT} -t ${readbst_app} -f ${CMAKE_CURRENT_SOURCE_DIR} -a -batch -a -topo"
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set_tests_properties(readbst_batch_test PROPERTIES DEPENDS readbst)
