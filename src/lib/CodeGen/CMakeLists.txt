
add_library(CodeGen STATIC
            CodeGen.cpp
            DirectCodeGen.cpp
            GraphCodeGen.cpp

            ExprTree.cpp
            ExprTreeLifter.cpp
            ElementLoopFuser.cpp
            StackExprRemover.cpp

            TheanoEmitter.cpp
            CEmitter.cpp
            )

add_library(obj.CodeGen OBJECT
            CodeGen.cpp
            DirectCodeGen.cpp
            GraphCodeGen.cpp

            ExprTree.cpp
            ExprTreeLifter.cpp
            ElementLoopFuser.cpp
            StackExprRemover.cpp

            TheanoEmitter.cpp
            CEmitter.cpp
            )
