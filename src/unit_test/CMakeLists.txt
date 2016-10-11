cmake_minimum_required(VERSION 2.8)

add_executable (hello hello.test.cpp)

add_executable (bitmap bitmap.test.cpp)
target_link_libraries (bitmap common)

add_executable (clock clock.test.cpp)
target_link_libraries (clock common)

add_executable (hash hash.test.cpp)
target_link_libraries (hash common)

add_executable (list list.test.cpp)

add_executable (singleton singleton.test.cpp)

