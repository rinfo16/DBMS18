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

add_executable (storage_service.test storage_service.test.cpp)
target_link_libraries (storage_service.test storage common gtest)

add_executable (relation relation.test.cpp)
target_link_libraries (relation storage common gtest)

add_executable (csv.test csv.test.cpp)
target_link_libraries (csv.test common)

add_executable (loader.test loader.test.cpp storage.test.cpp)
target_link_libraries (loader.test storage common boost_program_options boost_date_time)

add_executable (cmd.test cmd.test.cpp storage.test.cpp)
target_link_libraries (cmd.test storage common boost_program_options boost_date_time)

if (ENABLE_CODE_COVERAGE)
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")
  include(CodeCoverage)
  setup_target_for_coverage(coverage loader.test coverage)
endif()


