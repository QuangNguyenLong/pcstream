cmake_minimum_required(VERSION 3.21)

project(pcstreamTests LANGUAGES C)

include(../cmake/folders.cmake)

# ---- Dependencies ----

if(PROJECT_IS_TOP_LEVEL)
  find_package(pcstream REQUIRED)
  enable_testing()
endif()

# ---- Tests ----

add_executable(pcstream_test source/pcstream_test.c)
target_link_libraries(pcstream_test PRIVATE pcstream::pcstream)
target_compile_features(pcstream_test PRIVATE c_std_17)

add_test(NAME pcstream_test COMMAND pcstream_test)

# ---- End-of-file commands ----

add_folders(Test)
