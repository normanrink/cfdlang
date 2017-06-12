
# HACK: Add the directory where 'lex.yy.h' and 'lang.tab.hh' have been
# generated to the include path:
include_directories(${CFDLANG_EXTRA_OUTPUT_DIR})

add_executable(lex lex.cpp
                   $<TARGET_OBJECTS:obj.AST>
                   $<TARGET_OBJECTS:obj.Parser>)

add_executable(parse parse.cpp
                     $<TARGET_OBJECTS:obj.AST>
                     $<TARGET_OBJECTS:obj.Parser>)

add_executable(check check.cpp
                     $<TARGET_OBJECTS:obj.AST>
                     $<TARGET_OBJECTS:obj.Parser>
                     $<TARGET_OBJECTS:obj.Sema>)

add_executable(compile compile.cpp
                       $<TARGET_OBJECTS:obj.AST>
                       $<TARGET_OBJECTS:obj.CodeGen>
                       $<TARGET_OBJECTS:obj.Parser>
                       $<TARGET_OBJECTS:obj.Sema>)

