aux_source_directory(./ SRC_LIST)
add_executable (ns_client ${SRC_LIST})
target_link_libraries (ns_client boost_system)
