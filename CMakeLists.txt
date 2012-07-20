project(kvqc2d CXX)
cmake_minimum_required(VERSION 2.8)
enable_testing()

include(FindPkgConfig)
pkg_check_modules(PC_KVCPP REQUIRED libkvcpp2>=2.5.2a gsl>=1.8)
set(PC_KVCPP_LIBRARIES ${PC_KVCPP_LIBRARIES} kvalobs2_kvalobs kvalobs2_fileutil kvalobs2_milog kvalobs2_kvdb kvalobs2_miconfparser kvalobs2_kvskel)

include_directories(${PC_KVCPP_INCLUDE_DIRS})
add_definitions(${PC_KVCPP_CFLAGS_OTHER})

find_package(Boost COMPONENTS thread filesystem system REQUIRED)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -W")
endif()

add_subdirectory(src/helpers)
add_subdirectory(src/interpolation)
add_subdirectory(src/statisticalmean)
add_subdirectory(src)

add_subdirectory(test)
