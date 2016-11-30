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