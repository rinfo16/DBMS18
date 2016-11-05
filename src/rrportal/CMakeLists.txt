aux_source_directory(./ SRC_LIST)
add_executable (ns_server ${SRC_LIST})
target_link_libraries (
  ns_server
  parser
  storage
  executor
  common
  boost_system
  boost_program_options
  boost_date_time
)