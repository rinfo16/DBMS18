aux_source_directory(./ SRC_LIST)
add_executable (DBMS18 ${SRC_LIST})
target_link_libraries (
  DBMS18
  realizer
  storage
  executor
  parser
  common
  leveldb
  pthread
  boost_system
  boost_program_options
  boost_date_time
  boost_log
)

if (ENABLE_CODE_COVERAGE)
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")
  include(CodeCoverage)
  setup_target_for_coverage(owl_coverage DBMS18 owl_coverage)
endif()