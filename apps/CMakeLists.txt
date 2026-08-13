#------------------------------------------------------------------------------
#
# CMake build system for different applications
# Defines the command-line applications used by tests and coverage.
#
#------------------------------------------------------------------------------
#
# This file is licensed after LGPL v3
# Look at: https://www.gnu.org/licenses/lgpl-3.0.en.html for details
#
#------------------------------------------------------------------------------

set(APPS
  allbraces
  allperms
  allsubsets
  invperm
  is-baxters
  randbraces
  randperm
  readbst
  readtwin
  permloops
)

set(COMPUTABILITY_APPS ${APPS} PARENT_SCOPE)

foreach(APP ${APPS})
  add_executable(${APP} ${APP}.cc)
  target_compile_features(${APP} PRIVATE cxx_std_20)
  target_link_libraries(${APP} PRIVATE frame comblib)
endforeach()
