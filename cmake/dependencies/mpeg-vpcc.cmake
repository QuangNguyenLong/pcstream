# mpeg-vpcc.cmake
cmake_minimum_required(VERSION 3.21)

include(ExternalProject)

ExternalProject_Add(
    TMC2
    GIT_REPOSITORY https://github.com/MPEGGroup/mpeg-pcc-tmc2.git
    GIT_TAG release-v25.0
      INSTALL_COMMAND ""
      )

ExternalProject_Get_Property(TMC2 source_dir binary_dir)

set(TMC2_ROOT ${source_dir})
set(TMC2_LIBRARIES ${TMC2_ROOT}/lib)
link_directories(${TMC2_LIBRARIES})
