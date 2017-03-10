
# The following link explains how to integrate flex/bison
# into build processes that are controlled by CMake:
# https://cmake.org/pipermail/cmake/2002-September/003028.html


add_custom_target(FlexLexerSources echo "Generating lex.yy.c lex.yy.h"
                  DEPENDS ExtraOutputDirectory)

add_custom_command(TARGET FlexLexerSources
                   COMMAND flex
                   ARGS --outfile=${CFDLANG_EXTRA_OUTPUT_DIR}/lex.yy.cc
                        --header-file=${CFDLANG_EXTRA_OUTPUT_DIR}/lex.yy.h
                        ${CMAKE_CURRENT_SOURCE_DIR}/lang.l
                   DEPENDS lang.l)


add_custom_target(BisonParserSources echo "Generating lang.tab.cc lang.tab.hh"
                  DEPENDS ExtraOutputDirectory)

add_custom_command(TARGET BisonParserSources
                   COMMAND bison
                   ARGS --output=${CFDLANG_EXTRA_OUTPUT_DIR}/lang.tab.cc
                        --defines=${CFDLANG_EXTRA_OUTPUT_DIR}/lang.tab.hh
                        ${CMAKE_CURRENT_SOURCE_DIR}/lang.yy
                   DEPENDS lang.yy)

