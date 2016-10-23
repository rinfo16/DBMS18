cmake_minimum_required(VERSION 2.8)

add_executable (hello hello.test.cpp)
include_directories(../third_party/googletest/include)

add_executable (bitmap bitmap.test.cpp)
target_link_libraries (bitmap common gtest)

add_executable (clock clock.test.cpp)
target_link_libraries (clock common)

add_executable (hash hash.test.cpp)
target_link_libraries (hash common)

add_executable (list list.test.cpp)

add_executable (singleton singleton.test.cpp)

add_executable (file file.test.cpp)
target_link_libraries (file common storage gtest)

add_executable (storage_service storage_service.test.cpp)
target_link_libraries (storage_service storage common gtest)

add_executable (relation relation.test.cpp)
target_link_libraries (relation storage common gtest)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")
include(CodeCoverage)

if (ENABLE_CODE_COVERAGE)
  setup_target_for_coverage(coverage storage_service coverage)
endif()


