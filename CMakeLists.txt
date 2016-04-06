project(kvqc2d CXX)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

enable_testing()

include(GNUInstallDirs)
include(FindPkgConfig)
pkg_check_modules(PC_KVCPP REQUIRED libkvcpp>=4.0 gsl>=1.8)

include_directories(${PC_KVCPP_INCLUDE_DIRS})
add_definitions(${PC_KVCPP_CFLAGS_OTHER})

find_package(Boost COMPONENTS thread filesystem REQUIRED)

add_subdirectory(src/helpers)
add_subdirectory(src/interpolation)
add_subdirectory(src/statisticalmean)
add_subdirectory(src)

add_subdirectory(test)

configure_file(plu-autoupdate-from-stinfosys.in plu-autoupdate-from-stinfosys @ONLY)

install(PROGRAMS
  plu-update-from-stinfosys
  "${CMAKE_BINARY_DIR}/plu-autoupdate-from-stinfosys"
  DESTINATION lib/kvqc2d
)
