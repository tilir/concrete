#------------------------------------------------------------------------------
#
# CMake build system for allperms app test
#
#------------------------------------------------------------------------------
#
# This file is licensed after LGPL v3
# Look at: https://www.gnu.org/licenses/lgpl-3.0.en.html for details
#
#------------------------------------------------------------------------------

add_test(
  NAME allperms_test
  COMMAND bash -c "${TEST_SCRIPT} -t ${allperms_app} -f ${CMAKE_CURRENT_SOURCE_DIR}"
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set_tests_properties(allperms_test PROPERTIES DEPENDS allperms)