#------------------------------------------------------------------------------
#
# CMake build system for baxters app
# input: permutation, output: is it baxters
#
#------------------------------------------------------------------------------
#
# This file is licensed after LGPL v3
# Look at: https://www.gnu.org/licenses/lgpl-3.0.en.html for details
#
#------------------------------------------------------------------------------

add_test(
  NAME is-baxters_test
  COMMAND bash -c "${TEST_SCRIPT} -t ${is-baxters_app} -f ${CMAKE_CURRENT_SOURCE_DIR}"
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set_tests_properties(is-baxters_test PROPERTIES DEPENDS is-baxters)