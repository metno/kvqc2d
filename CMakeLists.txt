cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)  # use -std=c++20 instead of -std=gnu++20

#For rc version use MAJOR.MINOR.PATCH-rcN
#ex. 1.7.0-rc1
project(kvqc2d VERSION 1.6.2 LANGUAGES CXX)
MESSAGE("Configure kvqc2d ${PROJECT_VERSION}")

get_property(allTargets DIRECTORY PROPERTY BUILDSYSTEM_TARGETS)
foreach(t ${allTargets})
  get_property(std TARGET ${t} PROPERTY CXX_STANDARD)
  message(STATUS "Target ${t} CXX_STANDARD = ${std}")
endforeach()

set(CMAKE_BUILD_TYPE RelWithDebInfo)

enable_testing()

include(CMakeDependentOption)
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
get_property(allTargets DIRECTORY PROPERTY BUILDSYSTEM_TARGETS)
foreach(t ${allTargets})
  get_property(std TARGET ${t} PROPERTY CXX_STANDARD)
  message(STATUS "Target ${t} CXX_STANDARD = ${std}")
endforeach()
