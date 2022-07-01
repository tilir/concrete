#------------------------------------------------------------------------------
#
# CMake build system for different applications and tests
#
#------------------------------------------------------------------------------
#
# This file is licensed after LGPL v3
# Look at: https://www.gnu.org/licenses/lgpl-3.0.en.html for details
#
#------------------------------------------------------------------------------

set(APPS
  cellular-1d
)

foreach(APP ${APPS})
  add_executable(${APP} ${APP}.cc)
  target_compile_features(${APP} PRIVATE cxx_std_20)
  target_link_libraries(${APP} frame)
  target_link_libraries(${APP} drawer)
  target_link_libraries(${APP} SDL2)
endforeach()
