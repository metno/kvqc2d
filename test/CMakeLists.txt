
# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_EXTENSIONS ON)  # use -std=c++17 instead of -std=gnu++17

message("CMAKE_CXX_STANDARD = ${CXX_STANDARD}")
message("CMAKE_CXX_EXTENSIONS = ${CXX_EXTENSIONS}")

include_directories("${CMAKE_SOURCE_DIR}/src"
  "${CMAKE_CURRENT_SOURCE_DIR}"
  # include directory for generated files
  "${CMAKE_CURRENT_BINARY_DIR}/algorithms"
)

find_package(Boost COMPONENTS regex REQUIRED)

# FIND_PACKAGE(GTest QUIET)
# IF(NOT GTEST_FOUND)
#   MESSAGE("apparently no compiled GTest library, trying to build it")
#   FIND_FILE(GTEST_DIR src/gtest-all.cc /usr/src/gtest /usr/local/src/gtest /usr/src/googletest/googletest)
#   IF(NOT GTEST_DIR)
#     MESSAGE(FATAL_ERROR "could not find gtest-all.cc")
#   ENDIF()
#   GET_FILENAME_COMPONENT(GTEST_DIR ${GTEST_DIR} PATH)
#   GET_FILENAME_COMPONENT(GTEST_DIR ${GTEST_DIR} PATH)
#   ADD_SUBDIRECTORY(${GTEST_DIR} ${CMAKE_CURRENT_BINARY_DIR}/gtest EXCLUDE_FROM_ALL)
#   SET(GTEST_LIBRARY gtest)
#   SET(GTEST_MAIN_LIBRARY gtest_main)
# ENDIF()

FIND_PACKAGE(GTest QUIET)
IF(NOT GTEST_FOUND)
  MESSAGE("apparently no compiled GTest library, trying to build it")
  FIND_FILE(GTEST_DIR src/gtest-all.cc
    HINTS
    "${GTEST_ROOT}/src/gtest"
    /usr/src/googletest/googletest
    /usr/src/gmock/gtest
    /usr/src/gtest
    /usr/local/src/gtest
  )
  FIND_FILE(GTEST_DIR CMakeLists.txt ~/local/googletest /usr/src/googletest/)
  IF(NOT GTEST_DIR)
    MESSAGE(FATAL_ERROR "could not find googletest CMakeLists.txt")
  ENDIF()
  MESSAGE("Found googletest source dir: ${GTEST_DIR}")
  GET_FILENAME_COMPONENT(GTEST_DIR ${GTEST_DIR} PATH)
  include(ExternalProject)
  ExternalProject_Add(googletest
    SOURCE_DIR ${GTEST_DIR}
    BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/googletest
    CMAKE_ARGS
      -DCMAKE_CXX_STANDARD=17
      -DCMAKE_CXX_STANDARD_REQUIRED=ON
      -DCMAKE_CXX_EXTENSIONS=OFF
      -DBUILD_GMOCK=OFF
      -DINSTALL_GTEST=ON
      -DGTEST_HAS_ABSL=OFF
      -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/googletest-prefix
  )
  ExternalProject_Get_Property(googletest INSTALL_DIR)
  MESSAGE("GTest will be installed to ${INSTALL_DIR}")
  set(GTEST_INCLUDE_DIRS ${INSTALL_DIR}/include)
  set(GTEST_LIBRARY ${INSTALL_DIR}/lib/libgtest.a)
  set(GTEST_MAIN_LIBRARY ${INSTALL_DIR}/lib/libgtest_main.a)
  include_directories(${GTEST_INCLUDE_DIRS})
ELSE()
  SET(GTEST_LIBRARY gtest)
  SET(GTEST_MAIN_LIBRARY gtest_main)
ENDIF()
  
find_library(SQLITE3_LIBRARY sqlite3)

########### next target ###############

ADD_CUSTOM_COMMAND(
  OUTPUT algorithms/StatisticalMean_n212.cc
  COMMAND mkdir -p algorithms
  COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/algorithms/build_daymeans_212.py ${CMAKE_SOURCE_DIR}/data/normals_212.dat algorithms/StatisticalMean_n212.cc
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/algorithms/build_daymeans_212.py ${CMAKE_SOURCE_DIR}/data/normals_212.dat
)
ADD_CUSTOM_TARGET(generate_testfiles DEPENDS algorithms/StatisticalMean_n212.cc)

FILE(GLOB_RECURSE kvqc2dTest_SRCS "*.cc")
FOREACH(item ${kvqc2dTest_SRCS})
  IF(${item} MATCHES "dummy-kvserviced.cc")
    LIST(REMOVE_ITEM kvqc2dTest_SRCS ${item})
  ENDIF()
ENDFOREACH()

ADD_EXECUTABLE(kvqc2dTest
 ${kvqc2dTest_SRCS}
 ${CMAKE_CURRENT_BINARY_DIR}/algorithms/StatisticalMean_n212.cc
)
set_property(TARGET kvqc2dTest PROPERTY CXX_STANDARD 17)
set_property(TARGET kvqc2dTest PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET kvqc2dTest PROPERTY CXX_EXTENSIONS OFF)

ADD_DEPENDENCIES(kvqc2dTest generate_testfiles)

target_link_libraries(kvqc2dTest kvqc2d_1 interpolation statisticalmean kvqc2d_2 helpers
    ${PC_KVCPP_LIBRARIES}
    ${SQLITE3_LIBRARY}
    ${GTEST_LIBRARY} ${GTEST_MAIN_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_REGEX_LIBRARY}
)

add_test(
    NAME unitTest
    COMMAND kvqc2dTest
)


########### corba test ###############

SET(corbatest_SRCS
   dummy-kvserviced.cc
)

add_executable(corbatest ${corbatest_SRCS})

target_link_libraries(corbatest
    ${PC_KVCPP_LIBRARIES}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
)

add_test(
    NAME corbaTest
    COMMAND corbatest
)
