cmake_minimum_required(VERSION 3.21)
project(pcstreamApp C)


include(../cmake/folders.cmake)

if(PROJECT_IS_TOP_LEVEL)
  find_package(pcstream REQUIRED)
endif()

add_custom_target(run-app)

function(add_app NAME)
  add_executable("${NAME}" "${NAME}.c")
  target_link_libraries("${NAME}" PRIVATE pcstream::pcstream)
  target_compile_features("${NAME}" PRIVATE c_std_17)
  add_custom_target("run_${NAME}" COMMAND "${NAME}" VERBATIM)
  add_dependencies("run_${NAME}" "${NAME}")
  add_dependencies(run-app "run_${NAME}")
endfunction()

add_app(pcs)

# TODO: remember to write a wrapper for gpac/mpd.h core modules.
# add_example(genmpd)

add_folders(Application)
