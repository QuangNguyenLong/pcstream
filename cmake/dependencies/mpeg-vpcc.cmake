cmake_minimum_required(VERSION 3.21)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

find_package(Git REQUIRED)  # ensure GIT_EXECUTABLE is available

FetchContent_Declare(
  tmc2
  GIT_REPOSITORY https://github.com/MPEGGroup/mpeg-pcc-tmc2.git
  GIT_TAG        release-v25.0
  PATCH_COMMAND  ${GIT_EXECUTABLE} apply ${CMAKE_SOURCE_DIR}/patches/tmc2-install.patch
  &&             ${CMAKE_COMMAND} -E env bash ${CMAKE_SOURCE_DIR}/scripts/replace.sh <SOURCE_DIR>
  )

FetchContent_MakeAvailable(tmc2)
