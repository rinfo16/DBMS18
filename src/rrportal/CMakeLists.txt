aux_source_directory(./ SRC_LIST)
add_executable (Owl ${SRC_LIST})
target_link_libraries (
  Owl
  realizer
  storage
  executor
  parser
  common
  pthread
  boost_system
  boost_program_options
  boost_date_time
  boost_log
)

if (ENABLE_CODE_COVERAGE)
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")
  include(CodeCoverage)
  setup_target_for_coverage(owl_coverage Owl owl_coverage)
endif()