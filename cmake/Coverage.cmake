#------------------------------------------------------------------------------
#
# LLVM source-based coverage support.
#
#------------------------------------------------------------------------------

set(LLVM_COV llvm-cov CACHE STRING "llvm-cov executable")
set(LLVM_PROFDATA llvm-profdata CACHE STRING "llvm-profdata executable")

function(computability_enable_coverage_if_requested)
  if(NOT ENABLE_COVERAGE)
    return()
  endif()

  if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(FATAL_ERROR "ENABLE_COVERAGE requires Clang")
  endif()

  add_compile_options(
    -fprofile-instr-generate
    -fcoverage-mapping
  )

  add_link_options(
    -fprofile-instr-generate
    -fcoverage-mapping
  )
endfunction()

function(computability_add_coverage_targets_if_requested)
  if(NOT ENABLE_COVERAGE)
    return()
  endif()

  if(NOT COMPUTABILITY_APPS)
    message(FATAL_ERROR
      "COMPUTABILITY_APPS is empty; call "
      "computability_add_coverage_targets_if_requested() after add_subdirectory(apps)"
    )
  endif()

  set(COVERAGE_PROFILE_DIR ${CMAKE_BINARY_DIR}/profiles)
  set(COVERAGE_PROFDATA ${CMAKE_BINARY_DIR}/coverage.profdata)
  set(COVERAGE_HTML_DIR ${CMAKE_BINARY_DIR}/coverage-html)
  set(COVERAGE_IGNORE_REGEX "(/usr/include|${CMAKE_SOURCE_DIR}/tests)/")

  set(COVERAGE_TARGETS ${COMPUTABILITY_APPS} combinatorial-properties)
  set(COVERAGE_MAIN_OBJECT "")
  set(COVERAGE_OBJECT_ARGS)

  foreach(APP ${COVERAGE_TARGETS})
    if(COVERAGE_MAIN_OBJECT STREQUAL "")
      set(COVERAGE_MAIN_OBJECT "$<TARGET_FILE:${APP}>")
    else()
      list(APPEND COVERAGE_OBJECT_ARGS "-object=$<TARGET_FILE:${APP}>")
    endif()
  endforeach()

  set(COVERAGE_MERGE_SCRIPT ${CMAKE_BINARY_DIR}/merge-coverage.cmake)

  file(WRITE ${COVERAGE_MERGE_SCRIPT}
"file(GLOB PROFRAW_FILES \"\${PROFILE_DIR}/*.profraw\")

if(NOT PROFRAW_FILES)
  message(FATAL_ERROR \"No .profraw files found in \${PROFILE_DIR}\")
endif()

execute_process(
  COMMAND \"\${LLVM_PROFDATA}\" merge -sparse \${PROFRAW_FILES} -o \"\${PROFDATA}\"
  RESULT_VARIABLE MERGE_RESULT
)

if(NOT MERGE_RESULT EQUAL 0)
  message(FATAL_ERROR \"llvm-profdata merge failed\")
endif()

message(STATUS \"Wrote \${PROFDATA}\")
")

  add_custom_target(coverage-run
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${COVERAGE_PROFILE_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${COVERAGE_PROFILE_DIR}
    COMMAND ${CMAKE_COMMAND} -E env
            "LLVM_PROFILE_FILE=${COVERAGE_PROFILE_DIR}/%p-%m.profraw"
            ${CMAKE_CTEST_COMMAND} --test-dir ${CMAKE_BINARY_DIR} --output-on-failure
    DEPENDS ${COVERAGE_TARGETS}
    USES_TERMINAL
    VERBATIM
  )

  add_custom_target(coverage-merge
    COMMAND ${CMAKE_COMMAND}
            -DPROFILE_DIR=${COVERAGE_PROFILE_DIR}
            -DPROFDATA=${COVERAGE_PROFDATA}
            -DLLVM_PROFDATA=${LLVM_PROFDATA}
            -P ${COVERAGE_MERGE_SCRIPT}
    DEPENDS coverage-run
    USES_TERMINAL
    VERBATIM
  )

  add_custom_target(coverage-report
    COMMAND ${LLVM_COV} report
            ${COVERAGE_MAIN_OBJECT}
            ${COVERAGE_OBJECT_ARGS}
            -instr-profile=${COVERAGE_PROFDATA}
            -ignore-filename-regex=${COVERAGE_IGNORE_REGEX}
    DEPENDS coverage-merge
    USES_TERMINAL
    VERBATIM
  )

  add_custom_target(coverage-html
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${COVERAGE_HTML_DIR}
    COMMAND ${LLVM_COV} show
            ${COVERAGE_MAIN_OBJECT}
            ${COVERAGE_OBJECT_ARGS}
            -instr-profile=${COVERAGE_PROFDATA}
            -format=html
            -output-dir=${COVERAGE_HTML_DIR}
            -show-line-counts-or-regions
            -show-branches=count
            -ignore-filename-regex=${COVERAGE_IGNORE_REGEX}
    DEPENDS coverage-merge
    USES_TERMINAL
    VERBATIM
  )
endfunction()
