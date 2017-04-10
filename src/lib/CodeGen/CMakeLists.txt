
add_library(CodeGen STATIC
            CodeGen.cpp
            DirectCodeGen.cpp
            GraphCodeGen.cpp

            ExprTree.cpp
            ExprTreeLifter.cpp

            TheanoEmitter.cpp
            CEmitter.cpp
            )

