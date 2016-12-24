add_executable (hello hello.test.cpp)
include_directories(../third_party/googletest/include)

add_executable (bitmap bitmap.test.cpp)
target_link_libraries (bitmap common gtest)

add_executable (clock clock.test.cpp)
target_link_libraries (clock common)

add_executable (hash hash.test.cpp)
target_link_libraries (hash common)

add_executable (list list.test.cpp)



add_executable (csv.test csv.test.cpp)
target_link_libraries (csv.test common)

add_executable (parser.test parser.test.cpp)
target_link_libraries (parser.test parser common pthread boost_log)

if (ENABLE_CODE_COVERAGE)
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")
  include(CodeCoverage)
  setup_target_for_coverage(loader_coverage loader.test loader_coverage)
endif()


