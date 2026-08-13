#------------------------------------------------------------------------------
# CTest integration for exact CLI fixtures, real process pipelines, and
# in-process combinatorial property tests. Optional .err and .status files
# extend the basic .dat/.ans protocol for expected failures.
#------------------------------------------------------------------------------

find_program(RUBY_EXECUTABLE NAMES ruby)
if(NOT RUBY_EXECUTABLE)
  message(FATAL_ERROR "Ruby interpreter is required to run CLI tests")
endif()

set(TEST_RUNNER "${CMAKE_CURRENT_SOURCE_DIR}/scripts/test-run.rb")
set(PIPELINE_RUNNER "${CMAKE_CURRENT_SOURCE_DIR}/scripts/pipeline-run.rb")

function(add_fixture_tests SUITE APP)
  set(options)
  set(one_value_args)
  set(multi_value_args ARGS)
  cmake_parse_arguments(FIXTURE "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

  set(TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${SUITE}")
  file(GLOB INPUTS CONFIGURE_DEPENDS "${TEST_DIR}/*.dat")
  if(NOT INPUTS)
    message(FATAL_ERROR "No .dat fixtures found for ${SUITE}")
  endif()

  foreach(INPUT IN LISTS INPUTS)
    get_filename_component(CASE_NAME "${INPUT}" NAME_WE)
    set(EXPECTED "${TEST_DIR}/${CASE_NAME}.ans")
    if(NOT EXISTS "${EXPECTED}")
      message(FATAL_ERROR "Missing expected output: ${EXPECTED}")
    endif()

    set(COMMAND_ARGS
      --testfile "$<TARGET_FILE:${APP}>"
      --infile "${INPUT}"
      --etafile "${EXPECTED}"
    )
    if(EXISTS "${TEST_DIR}/${CASE_NAME}.err")
      list(APPEND COMMAND_ARGS --errfile "${TEST_DIR}/${CASE_NAME}.err")
    endif()
    if(EXISTS "${TEST_DIR}/${CASE_NAME}.status")
      list(APPEND COMMAND_ARGS --statusfile "${TEST_DIR}/${CASE_NAME}.status")
    endif()
    foreach(ARG IN LISTS FIXTURE_ARGS)
      list(APPEND COMMAND_ARGS --arg "${ARG}")
    endforeach()

    add_test(
      NAME "fixture.${SUITE}.${CASE_NAME}"
      COMMAND "${RUBY_EXECUTABLE}" "${TEST_RUNNER}" ${COMMAND_ARGS}
    )
    set_tests_properties("fixture.${SUITE}.${CASE_NAME}" PROPERTIES LABELS "fixture;${SUITE}")
  endforeach()
endfunction()

add_fixture_tests(readbst readbst)
add_fixture_tests(readbst-back readbst ARGS -back)
add_fixture_tests(readbst-topo readbst ARGS -topo)
add_fixture_tests(readbst-ibrac readbst ARGS -ibrac)
add_fixture_tests(readbst-batch readbst ARGS -batch -topo)

add_fixture_tests(permloops permloops)
add_fixture_tests(invperm invperm)
add_fixture_tests(allperms allperms)
add_fixture_tests(baxters is-baxters)
add_fixture_tests(baxters-batch is-baxters ARGS -batch -combine)
add_fixture_tests(baxters-filt is-baxters ARGS -batch -filter)

add_fixture_tests(readtwin readtwin)
add_fixture_tests(readtwin-baxters readtwin ARGS -baxters)
add_fixture_tests(allbraces allbraces)
add_fixture_tests(allsubsets allsubsets)
add_fixture_tests(randperm-range randperm ARGS -first=-2 -last=2 -seed=17)
add_fixture_tests(randperm-single randperm ARGS -first=3 -last=3 -seed=17 -verbose)
add_fixture_tests(randbraces randbraces ARGS -n=6 -seed=17)
add_fixture_tests(randbraces-ones randbraces ARGS -n=4 -seed=2 -ones)
add_fixture_tests(readbst-duplicate readbst)
add_fixture_tests(readtwin-duplicate readtwin)
add_fixture_tests(readbst-conflicting readbst ARGS -dot -topo)
add_fixture_tests(readbst-malformed readbst)
add_fixture_tests(readtwin-malformed readtwin)
add_fixture_tests(readtwin-truncated readtwin)
add_fixture_tests(cli-error-allbraces allbraces ARGS -definitely-invalid)
add_fixture_tests(cli-error-allperms allperms ARGS -definitely-invalid)
add_fixture_tests(cli-error-allsubsets allsubsets ARGS -definitely-invalid)
add_fixture_tests(cli-error-invperm invperm ARGS -definitely-invalid)
add_fixture_tests(cli-error-baxters is-baxters ARGS -definitely-invalid)
add_fixture_tests(cli-error-randbraces randbraces ARGS -definitely-invalid)
add_fixture_tests(cli-error-randperm randperm ARGS -definitely-invalid)
add_fixture_tests(cli-error-readbst readbst ARGS -definitely-invalid)
add_fixture_tests(cli-error-readtwin readtwin ARGS -definitely-invalid)
add_fixture_tests(cli-error-permloops permloops ARGS -definitely-invalid)
add_fixture_tests(cli-invalid-value randperm ARGS -seed=abc)
add_fixture_tests(cli-help-substring randperm ARGS -helpful)
add_fixture_tests(invperm-invalid-domain invperm)
add_fixture_tests(invperm-duplicate invperm)
add_fixture_tests(invperm-truncated invperm)
add_fixture_tests(readbst-unbalanced readbst ARGS -ibrac)
add_fixture_tests(randperm-invalid-range randperm ARGS -first=3 -last=2)
add_fixture_tests(randbraces-invalid-size randbraces ARGS -n=-1)
add_fixture_tests(allsubsets-zero allsubsets)
add_fixture_tests(allsubsets-invalid-size allsubsets)
add_fixture_tests(allperms-invalid-size allperms)
add_fixture_tests(allbraces-invalid-size allbraces)
add_fixture_tests(permloops-invalid-size permloops)
add_fixture_tests(readtwin-invalid-size readtwin)
add_fixture_tests(readbst-invalid-size readbst)

add_executable(combinatorial-properties combinatorial-properties.cc)
target_compile_features(combinatorial-properties PRIVATE cxx_std_20)
target_compile_definitions(combinatorial-properties PRIVATE CHECKS)
target_link_libraries(combinatorial-properties PRIVATE frame comblib)
add_test(NAME properties.combinatorial COMMAND combinatorial-properties)
set_tests_properties(properties.combinatorial PROPERTIES LABELS "properties")

function(add_pipeline_test NAME PRODUCER CONSUMER)
  set(options)
  set(one_value_args)
  set(multi_value_args PRODUCER_ARGS CONSUMER_ARGS)
  cmake_parse_arguments(PIPE "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

  set(INPUT "${CMAKE_CURRENT_SOURCE_DIR}/pipelines/${NAME}.dat")
  set(EXPECTED "${CMAKE_CURRENT_SOURCE_DIR}/pipelines/${NAME}.ans")
  if(NOT EXISTS "${INPUT}" OR NOT EXISTS "${EXPECTED}")
    message(FATAL_ERROR "Missing pipeline fixture for ${NAME}")
  endif()

  set(COMMAND_ARGS
    --producer "$<TARGET_FILE:${PRODUCER}>"
    --consumer "$<TARGET_FILE:${CONSUMER}>"
    --infile "${INPUT}"
    --etafile "${EXPECTED}"
  )
  foreach(ARG IN LISTS PIPE_PRODUCER_ARGS)
    list(APPEND COMMAND_ARGS --producer-arg "${ARG}")
  endforeach()
  foreach(ARG IN LISTS PIPE_CONSUMER_ARGS)
    list(APPEND COMMAND_ARGS --consumer-arg "${ARG}")
  endforeach()

  add_test(
    NAME "pipeline.${NAME}"
    COMMAND "${RUBY_EXECUTABLE}" "${PIPELINE_RUNNER}" ${COMMAND_ARGS}
  )
  set_tests_properties("pipeline.${NAME}" PROPERTIES LABELS "pipeline;${NAME}")
endfunction()

add_pipeline_test(allperms-baxters allperms is-baxters
  CONSUMER_ARGS -batch -combine)
add_pipeline_test(randperm-invperm randperm invperm
  PRODUCER_ARGS -last=8 -seed=17)
add_pipeline_test(randperm-readbst randperm readbst
  PRODUCER_ARGS -last=8 -seed=17)
add_pipeline_test(randperm-permloops randperm permloops
  PRODUCER_ARGS -last=8 -seed=17)
add_pipeline_test(allbraces-readbst allbraces readbst
  CONSUMER_ARGS -batch -ibrac -topo)
add_pipeline_test(allperms-readtwin allperms readtwin
  CONSUMER_ARGS -batch -baxters)
