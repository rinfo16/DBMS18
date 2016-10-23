#ifndef DEFINE_H_
#define DEFINE_H_

#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>

#define PAGE_SIZE                 8192
#define PAGE_NUMBER_PER_EXTENT    320
#define MAX_PAGE_NUMBER_PER_FILE  8192000
#define MAX_EXTENT_COUNT          40000
#define BUFFER_POOL_SIZE          800000000
#define DATA_DIRECTORY            "/tmp"
#define FLIP_NUMBER               20161012
#define SEGMENT_DESCRIPT_FILE_NO  0
#define DATA_FILE_NO              1
#define FILE_HEADER_PAGE_NO       0
#define OUT

#define STR_NAME                  "NAME"
#define STR_ID                    "ID"
#define STR_RELATION_ID           "RELATION_ID"
#define STR_MAX_LENGTH            "MAX_LENGTH"
#define STR_DATA_TYPE             "DATA_TYPE"
#define STR_IS_NULL               "IS_NULL"
#define STR_IS_VARIABLE           "IS_VARIABLE"
#define STR_FIRST_DATA_PAGE       "FIRST_DATA_PAGE"
#define STR_LAST_DATA_PAGE        "LAST_DATA_PAGE"
#define STR_FIRST_EXTENT          "FIRST_EXTENT"
#define STR_LAST_EXTENT           "LAST_EXTENT"
#define STR_FILE_NO               "FILE_NO"
#define STR_PAGE_NO               "PAGE_NO"
#define STR_ATTRIBUTE_LIST        "ATTRIBUTE_LIST"



typedef uint32_t fileno_t;
typedef uint32_t segmentno_t;
typedef uint32_t extentno_t;
typedef uint32_t pageno_t;
typedef uint32_t slotno_t;
typedef uint32_t relationid_t;
typedef uint32_t attributeid_t;


#endif // DEFINE_H_
