
# HACK: Add the directory where 'lex.yy.h' and 'lang.tab.hh' have been
# generated to the include path:
include_directories(${CFDLANG_EXTRA_OUTPUT_DIR})

add_library(Runtime STATIC
                    TensorCodeGen.cpp
                    TensorContext.cpp
                    TensorExecution.cpp
                    TensorKernel.cpp)
