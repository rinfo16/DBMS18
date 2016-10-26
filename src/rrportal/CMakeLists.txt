aux_source_directory(./ SRC_LIST)
add_executable (ns_server ${SRC_LIST})
target_link_libraries (ns_server boost_system)