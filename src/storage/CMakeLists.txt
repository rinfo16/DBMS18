include_directories(storage/leveldb/)
add_subdirectory(leveldb)
aux_source_directory(./ SRC_LIST)
add_library(storage ${SRC_LIST})
