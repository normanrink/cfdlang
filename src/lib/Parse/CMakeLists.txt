
# Add the directory where 'lex.yy.h' and 'lang.tab.hh' have been
# generated to the include path:
include_directories(${CFDLANG_EXTRA_OUTPUT_DIR})

set_source_files_properties(${CFDLANG_EXTRA_OUTPUT_DIR}/lex.yy.cc GENERATED)
set_source_files_properties(${CFDLANG_EXTRA_OUTPUT_DIR}/lang.tab.cc GENERATED)

add_library(obj.Parser OBJECT
            Lexer.cpp
            Parser.cpp
            ${CFDLANG_EXTRA_OUTPUT_DIR}/lex.yy.cc
            ${CFDLANG_EXTRA_OUTPUT_DIR}/lang.tab.cc)
add_dependencies(obj.Parser FlexLexerSources
                            BisonParserSources)
