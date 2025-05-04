cmake_minimum_required(VERSION 3.21)

include(ExternalProject)

set(GOOGLE_DRACO_PREFIX ${CMAKE_SOURCE_DIR}/dependencies)

ExternalProject_Add(draco
  GIT_REPOSITORY https://github.com/google/draco.git
  GIT_TAG        1.5.7
  UPDATE_COMMAND ""
  INSTALL_DIR    ${GOOGLE_DRACO_PREFIX}
  CMAKE_ARGS     -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
  STEP_TARGETS   build install
)

# include(FetchContent)
# 
# FetchContent_Declare(
#     draco
#     GIT_REPOSITORY https://github.com/google/draco.git
#     GIT_TAG        1.5.7
# )
# 
# FetchContent_GetProperties(draco)
# 
# if(NOT draco_POPULATED)
#     FetchContent_Populate(draco)
#     add_subdirectory(${draco_SOURCE_DIR} ${draco_BINARY_DIR})
# endif()
# 
# set(draco_INCLUDE_DIR ${draco_SOURCE_DIR}/src)
# set(draco_LIBRARIES draco::draco)
# 
# 
# # FetchContent_MakeAvailable(draco)
# # 
# # set(DRACO_LIBS draco::draco)
# # 
# 
# # message(bumbum=${draco_SOURCE_DIR})
