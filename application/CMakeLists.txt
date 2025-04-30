cmake_minimum_required(VERSION 3.21)
project(pcstreamApp C)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GPAC REQUIRED gpac)

# Automatically get static link flags using pkg-config
execute_process(
    COMMAND pkg-config --libs --static gpac
    OUTPUT_VARIABLE GPAC_STATIC_LIBS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Print the flags to verify
message(STATUS "GPAC_STATIC_LIBS: ${GPAC_STATIC_LIBS}")

# genmpd app
add_executable(genmpd genmpd.c)
target_link_libraries(genmpd PRIVATE pcstream::pcstream ${GPAC_STATIC_LIBS})
target_compile_features(genmpd PRIVATE c_std_17)

# pcs app
add_executable(pcs pcs.c)
target_link_libraries(pcs PRIVATE pcstream::pcstream)
target_compile_features(pcs PRIVATE c_std_17)

