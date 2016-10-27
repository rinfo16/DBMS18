cmake_minimum_required(VERSION 3.1)

aux_source_directory(./ SRC_LIST)


find_program(FLEX flex DOC "Path to the flex lexical analyser generator.")
find_program(BISON bison DOC "Path to the bison parser generator.")

# Custom commands to invoke flex and bison.

set(BISON_DEBUG "")
if ("${CMAKE_BUILD_TYPE}" STREQUAL "DEBUG")
  set(BISON_DEBUG --verbose --debug)
endif()

set (FLEX_OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/lex.parser.cpp)
set (BISON_OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/parser.tab.cpp)

add_custom_command(OUTPUT ${FLEX_OUTPUT}
                   COMMAND ${FLEX} --outfile=${FLEX_OUTPUT} --c++ ${CMAKE_CURRENT_SOURCE_DIR}/scanner.ll 
                   MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/scanner.ll
                   COMMENT "Generating lexer"
                   VERBATIM)
add_custom_command(OUTPUT ${BISON_OUTPUT}
                   COMMAND ${BISON} --output=${BISON_OUTPUT} ${CMAKE_CURRENT_SOURCE_DIR}/parser.yy ${BISON_DEBUG}
                   MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/parser.yy
                   COMMENT "Generating parser"
                   VERBATIM)

add_library (parser ${SRC_LIST} ${FLEX_OUTPUT} ${BISON_OUTPUT})
