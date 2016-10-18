cmake_minimum_required(VERSION 3.1)
project (Nutshell)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE DEBUG)
endif(NOT CMAKE_BUILD_TYPE)

set (CMAKE_CXX_STANDARD 11
    CACHE STRING "build use C++11 standard."
    FORCE)
    
message(STATUS "Build type is ${CMAKE_BUILD_TYPE}")

set(ENABLE_CODE_COVERAGE false)
if ("${CMAKE_BUILD_TYPE}" STREQUAL "COVERAGE_DEBUG")
  set(CMAKE_BUILD_TYPE DEBUG)
  set(ENABLE_CODE_COVERAGE true)
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "COVERAGE_RELEASE")
  set(CMAKE_BUILD_TYPE RELEASE)
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

add_subdirectory(unit_test)
add_subdirectory(storage)
add_subdirectory(common)
add_subdirectory(third_party)
