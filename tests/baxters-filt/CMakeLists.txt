#------------------------------------------------------------------------------
#
# CMake build system for baxters app, batch + filter
# tests for batch input, lot of permutations to lot of Baxter permutations
#
#------------------------------------------------------------------------------
#
# This file is licensed after LGPL v3
# Look at: https://www.gnu.org/licenses/lgpl-3.0.en.html for details
#
#------------------------------------------------------------------------------

add_test(
  NAME baxters_batch_filter_test
  COMMAND bash -c "${TEST_SCRIPT} -t ${is-baxters_app} -f ${CMAKE_CURRENT_SOURCE_DIR} -a -batch -a -filter"
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set_tests_properties(baxters_batch_filter_test PROPERTIES DEPENDS ${is-baxters_app})
