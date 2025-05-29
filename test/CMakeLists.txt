cmake_minimum_required(VERSION 3.21)

project(pcstreamTests LANGUAGES C)

include(../cmake/folders.cmake)

# ---- Dependencies ----

if(PROJECT_IS_TOP_LEVEL)
  find_package(pcstream REQUIRED)
  enable_testing()
endif()

# ---- Tests ----

set(TEST_ASSETS ${CMAKE_SOURCE_DIR}/assets/test)

function(pcstream_add_test NAME ARGS)
  add_executable("${NAME}" "source/${NAME}.c")
  target_link_libraries("${NAME}" PRIVATE pcstream::pcstream)
  target_compile_features("${NAME}" PRIVATE c_std_17)
  add_test(NAME ${NAME} COMMAND ${NAME} ${ARGS})
endfunction()

pcstream_add_test(algorithm_test "${TEST_ASSETS}/algorithm/info.init.txt")
pcstream_add_test(buffer_test "")
pcstream_add_test(bw_estimator_test "")
pcstream_add_test(gof_test "")
pcstream_add_test(http_test "https://raw.githubusercontent.com/filrg/pcstream/refs/heads/main/assets/test/longdress.seg1.r1.bin")
pcstream_add_test(lod_selector_test "${TEST_ASSETS}/algorithm/info.init.txt")
pcstream_add_test(mesh_test "${TEST_ASSETS}/hull.ply")
# pcstream_add_test(pcstream_test)
# pcstream_add_test(point_cloud_test)
## pcstream_add_test(request_handler_test "https://127.0.0.1:8080/manifest.mpd https://127.0.0.1:8080/manifest-info.mpd https://127.0.0.1:8080/manifest-hull.mpd https://127.0.0.1:8080")
# pcstream_add_test(vec2f_test)
# pcstream_add_test(vec3f_test)
# pcstream_add_test(video_decoder_test)
# pcstream_add_test(viewport_estimator_test)
# pcstream_add_test(visibility_computer_test)

# ---- End-of-file commands ----

add_folders(Test)
