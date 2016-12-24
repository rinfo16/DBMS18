cmake_minimum_required(VERSION 2.7)
project (owl)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE DEBUG)
endif(NOT CMAKE_BUILD_TYPE)

if (CMAKE_VERSION VERSION_LESS "3.1")
  if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set (CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
  endif ()
else ()
    set (CMAKE_CXX_STANDARD 11)
endif ()

message(STATUS "Build type is ${CMAKE_BUILD_TYPE}")

set(ENABLE_CODE_COVERAGE false)
if ("${CMAKE_BUILD_TYPE}" STREQUAL "CoverageDebug")
  set(CMAKE_BUILD_TYPE Debug)
  set(ENABLE_CODE_COVERAGE true)
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "CoverageRelease")
  set(CMAKE_BUILD_TYPE Release)
  set(ENABLE_CODE_COVERAGE true)
endif()

set(CXX_COVERAGE_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage")


message(STATUS "ENABLE_CODE_COVERAGE: ${ENABLE_CODE_COVERAGE}")

if (ENABLE_CODE_COVERAGE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_COVERAGE_FLAGS}"
      CACHE STRING "Flags used by the C++ compiler during coverage builds."
    FORCE)
  set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_COVERAGE_FLAGS}"
      CACHE STRING "Flags used by the C compiler during coverage builds."
    FORCE)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(include)
include_directories(storage/leveldb/include)
include_directories(storage/leveldb)

add_subdirectory(executor)
add_subdirectory(common)
add_subdirectory(client)
add_subdirectory(realizer)
add_subdirectory(rrportal)
add_subdirectory(parser)
add_subdirectory(postgres)
add_subdirectory(storage)
add_subdirectory(third_party)
add_subdirectory(unit_test)
