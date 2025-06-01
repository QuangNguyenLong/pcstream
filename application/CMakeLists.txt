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

# add_app(pcs)

file(GLOB PCS_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
add_executable(pcs ${PCS_SOURCE})

find_package(PkgConfig REQUIRED)
pkg_check_modules(gpac REQUIRED IMPORTED_TARGET gpac)
pkg_check_modules(curl REQUIRED IMPORTED_TARGET libcurl)

target_link_libraries(
    pcs PRIVATE
    pcstream::pcstream
    PkgConfig::gpac
    PkgConfig::curl
    )

target_compile_features(pcs PRIVATE c_std_17)
add_custom_target("run_pcs" COMMAND pcs VERBATIM)
add_dependencies("run_pcs" "pcs")
add_dependencies(run-app "run_pcs")


# TODO: remember to write a wrapper for gpac/mpd.h core modules.
# add_example(genmpd)

add_folders(Application)
