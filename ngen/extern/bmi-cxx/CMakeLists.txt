cmake_minimum_required(VERSION 3.12)

project(bmicxx LANGUAGES CXX)

set(BMI_VERSION 2.0)
include(GNUInstallDirs)

configure_file(${CMAKE_SOURCE_DIR}/${CMAKE_PROJECT_NAME}.pc.cmake
  ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.pc)

install(
  FILES bmi.hxx
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(
  FILES ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.pc
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)
