aux_source_directory(./ SRC_LIST)
add_executable (nutshell ${SRC_LIST})
target_link_libraries (
  nutshell
  parser
  storage
  executor
  common
  boost_system
  boost_program_options
  boost_date_time
)