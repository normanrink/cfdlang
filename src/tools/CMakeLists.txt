
# HACK: Add the directory where 'lex.yy.h' and 'lang.tab.hh' have been
# generated to the include path:
include_directories(${CFDLANG_EXTRA_OUTPUT_DIR})

add_executable(lex lex.cpp)
target_link_libraries(lex Parser)

add_executable(parse parse.cpp)
target_link_libraries(parse AST
                            Parser)

add_executable(check check.cpp)
target_link_libraries(check AST
                            Parser
                            Sema)

add_executable(compile compile.cpp)
target_link_libraries(compile AST
                              CodeGen
                              Parser
                              Sema)

add_executable(context_test context_test.cpp)
target_link_libraries(context_test AST
                                   CodeGen
                                   Parser
				     Runtime
                                   Sema)

