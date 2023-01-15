#------------------------------------------------------------------------------
#
# CMake build system for readtwin app test
#
#------------------------------------------------------------------------------
#
# This file is licensed after LGPL v3
# Look at: https://www.gnu.org/licenses/lgpl-3.0.en.html for details
#
#------------------------------------------------------------------------------

SET(TESTNAME readtwin_test)
SET(APPNAME ${readtwin_app})

add_test(
  NAME ${TESTNAME}
  COMMAND bash -c "${TEST_SCRIPT} -t ${APPNAME} -f ${CMAKE_CURRENT_SOURCE_DIR}"
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set_tests_properties(${TESTNAME} PROPERTIES DEPENDS ${APPNAME})