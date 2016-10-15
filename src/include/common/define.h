#ifndef DEFINE_H_
#define DEFINE_H_

#include <stdint.h>
#include <stdlib.h>

#define PAGE_SIZE                 8192
#define PAGE_NUMBER_PER_EXTENT    32
#define MAX_PAGE_NUMBER_PER_FILE  8192000
#define MAX_EXTENT_COUNT          8888
#define BUFFER_POOL_SIZE          800000000
#define DATA_DIRECTORY            "/tmp"
#define FLIP_NUMBER               20161012
#define SEGMENT_DESCRIPT_FILE_NO  0
#define DATA_FILE_NO              1
#define FILE_HEADER_PAGE_NO       0
typedef uint32_t fileno_t;
typedef uint32_t segmentno_t;
typedef uint32_t extentno_t;
typedef uint32_t pageno_t;
typedef uint32_t slotno_t;
typedef uint32_t relationid_t;
typedef uint32_t attributeid_t;
typedef uint64_t cursor_t;

#endif // DEFINE_H_
