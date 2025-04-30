if(PROJECT_IS_TOP_LEVEL)
  set(
      CMAKE_INSTALL_INCLUDEDIR "include/pcstream-${PROJECT_VERSION}"
      CACHE STRING ""
  )
  set_property(CACHE CMAKE_INSTALL_INCLUDEDIR PROPERTY TYPE PATH)
endif()

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# find_package(<package>) call for consumers to find this project
set(package pcstream)

install(
    DIRECTORY
    include/
    "${PROJECT_BINARY_DIR}/export/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT pcstream_Development
)

install(
    TARGETS pcstream_pcstream
    EXPORT pcstreamTargets
    RUNTIME #
    COMPONENT pcstream_Runtime
    LIBRARY #
    COMPONENT pcstream_Runtime
    NAMELINK_COMPONENT pcstream_Development
    ARCHIVE #
    COMPONENT pcstream_Development
    INCLUDES #
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

write_basic_package_version_file(
    "${package}ConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion
)

# Allow package maintainers to freely override the path for the configs
set(
    pcstream_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/${package}"
    CACHE STRING "CMake package config location relative to the install prefix"
)
set_property(CACHE pcstream_INSTALL_CMAKEDIR PROPERTY TYPE PATH)
mark_as_advanced(pcstream_INSTALL_CMAKEDIR)

install(
    FILES cmake/install-config.cmake
    DESTINATION "${pcstream_INSTALL_CMAKEDIR}"
    RENAME "${package}Config.cmake"
    COMPONENT pcstream_Development
)

install(
    FILES "${PROJECT_BINARY_DIR}/${package}ConfigVersion.cmake"
    DESTINATION "${pcstream_INSTALL_CMAKEDIR}"
    COMPONENT pcstream_Development
)

install(
    EXPORT pcstreamTargets
    NAMESPACE pcstream::
    DESTINATION "${pcstream_INSTALL_CMAKEDIR}"
    COMPONENT pcstream_Development
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
