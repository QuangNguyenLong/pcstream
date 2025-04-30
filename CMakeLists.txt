cmake_minimum_required(VERSION 3.21)

include(cmake/prelude.cmake)

project(
    pcstream
    VERSION 0.1.0
    DESCRIPTION "A point cloud streaming tool"
    HOMEPAGE_URL "https://filrg.github.io/pcstream"
    LANGUAGES C CXX
)

include(cmake/variables.cmake)

OPTION(USE_MPEG_VPCC_CODEC "Use MPEG-VPCC to encode and decode video" TRUE)

# ---- Build dependencies ----

if(USE_MPEG_VPCC_CODEC)
    include(cmake/dependencies/mpeg-vpcc.cmake)
endif()

# ---- Declare library ----

file(GLOB PCSTREAM_SHARED_SOURCE
     ${PROJECT_SOURCE_DIR}/source/*
     )

add_library(pcstream_pcstream ${PCSTREAM_SHARED_SOURCE})
add_library(pcstream::pcstream ALIAS pcstream_pcstream)


find_package(PkgConfig REQUIRED)
pkg_check_modules(GPAC REQUIRED IMPORTED_TARGET gpac)
pkg_check_modules(LIBCURL REQUIRED IMPORTED_TARGET libcurl)

find_library(MATH_LIB m REQUIRED)

if(USE_MPEG_VPCC_CODEC)
    add_dependencies(pcstream_pcstream TMC2)
    target_include_directories(pcstream_pcstream
        PRIVATE
        ${TMC2_ROOT}/source/lib/PccLibCommon/include
        ${TMC2_ROOT}/source/lib/PccLibDecoder/include
        ${TMC2_ROOT}/source/lib/PccLibMetrics/include
        ${TMC2_ROOT}/source/lib/PccLibConformance/include
        ${TMC2_ROOT}/source/lib/PccLibBitstreamCommon/include
        ${TMC2_ROOT}/source/lib/PccLibBitstreamReader/include
        ${TMC2_ROOT}/source/lib/PccLibConformance/include
        ${TMC2_ROOT}/dependencies/tbb/include
        )
    target_link_libraries(pcstream_pcstream PRIVATE
        PccLibDecoder
        PccLibCommon
        tbb_static
        PccLibColorConverter
        PccLibVideoDecoder
        PccLibHevcParser
        TLibDecoder
        TLibCommon
        PccLibAvcParser
        PccLibBitstreamCommon
        PccLibBitstreamReader
        )
endif()
target_link_libraries(pcstream_pcstream PRIVATE PkgConfig::GPAC PkgConfig::LIBCURL ${MATH_LIB})

include(GenerateExportHeader)
generate_export_header(
    pcstream_pcstream
    BASE_NAME pcstream
    EXPORT_FILE_NAME export/pcstream/pcstream_export.h
)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(pcstream_pcstream PUBLIC PCSTREAM_STATIC_DEFINE)
endif()

set_target_properties(
    pcstream_pcstream PROPERTIES
    C_VISIBILITY_PRESET hidden
    VERSION "${PROJECT_VERSION}"
    SOVERSION "${PROJECT_VERSION_MAJOR}"
    EXPORT_NAME pcstream
    OUTPUT_NAME pcstream
)

target_include_directories(
    pcstream_pcstream ${warning_guard}
    PUBLIC
    "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_include_directories(
    pcstream_pcstream SYSTEM
    PUBLIC
    "\$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/export>"
)

target_compile_features(pcstream_pcstream PUBLIC c_std_17)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Application ----

if(PROJECT_IS_TOP_LEVEL)
    option(BUILD_APPLICATIONS "Build applications tree." "${pcstream_DEVELOPER_MODE}")
    if(BUILD_APPLICATIONS)
    add_subdirectory(application)
  endif()
endif()

# ---- Examples ----

if(PROJECT_IS_TOP_LEVEL)
  option(BUILD_EXAMPLES "Build examples tree." "${pcstream_DEVELOPER_MODE}")
  if(BUILD_EXAMPLES)
    add_subdirectory(example)
  endif()
endif()

# ---- Developer mode ----

if(NOT pcstream_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of pcstream"
  )
endif()

include(cmake/dev-mode.cmake)
