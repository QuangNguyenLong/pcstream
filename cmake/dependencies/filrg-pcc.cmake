cmake_minimum_required(VERSION 3.21)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare(
  pcc
  GIT_REPOSITORY https://github.com/filrg/pcc.git
  GIT_TAG        main
  )

FetchContent_MakeAvailable(pcc)

