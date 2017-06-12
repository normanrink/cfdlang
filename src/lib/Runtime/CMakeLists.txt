
# HACK: Add the directory where 'lex.yy.h' and 'lang.tab.hh' have been
# generated to the include path:
include_directories(${CFDLANG_EXTRA_OUTPUT_DIR})

add_library(obj.Runtime OBJECT
                        TensorCodeGen.cpp
                        TensorContext.cpp
                        TensorExecution.cpp
                        TensorKernel.cpp
                        TensorWrappers.cpp)
