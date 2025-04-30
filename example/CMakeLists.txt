cmake_minimum_required(VERSION 3.21)

project(pcstreamExamples C)

include(../cmake/folders.cmake)

if(PROJECT_IS_TOP_LEVEL)
  find_package(pcstream REQUIRED)
endif()

add_custom_target(run-examples)

function(add_example NAME)
  add_executable("${NAME}" "${NAME}.c")
  target_link_libraries("${NAME}" PRIVATE pcstream::pcstream)
  target_compile_features("${NAME}" PRIVATE c_std_17)
  add_custom_target("run_${NAME}" COMMAND "${NAME}" VERBATIM)
  add_dependencies("run_${NAME}" "${NAME}")
  add_dependencies(run-examples "run_${NAME}")
endfunction()

add_example(hull)
add_example(tmc2_decode)

add_folders(Example)
