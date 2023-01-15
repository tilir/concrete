#------------------------------------------------------------------------------
#
# CMake build system for different applications
# set env like readbst_app for parent scopr to use in testing
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
  permloops
  invperm
  is-baxters
  allperms
)

foreach(APP ${APPS})
  set(${APP}_app ${CMAKE_BINARY_DIR}/apps/${APP} PARENT_SCOPE)
  add_executable(${APP} ${APP}.cc)
  target_compile_features(${APP} PRIVATE cxx_std_20)
  target_link_libraries(${APP} frame)
  target_link_libraries(${APP} treelib)
  target_link_libraries(${APP} permlib)
if(VISUAL)
  target_link_libraries(${APP} drawer)
  target_link_libraries(${APP} SDL2)
endif()

endforeach()
