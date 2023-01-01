#------------------------------------------------------------------------------
#
# CMake build system for different applications
#
#------------------------------------------------------------------------------
#
# This file is licensed after LGPL v3
# Look at: https://www.gnu.org/licenses/lgpl-3.0.en.html for details
#
#------------------------------------------------------------------------------

set(APPS
  # cellular-1d <--- split to visual and normal parts
  readbst
  randperm
  randbraces
)

foreach(APP ${APPS})
  add_executable(${APP} ${APP}.cc)
  target_compile_features(${APP} PRIVATE cxx_std_20)
  target_link_libraries(${APP} frame)
  target_link_libraries(${APP} treelib)
if(VISUAL)
  target_link_libraries(${APP} drawer)
  target_link_libraries(${APP} SDL2)
endif()
endforeach()
