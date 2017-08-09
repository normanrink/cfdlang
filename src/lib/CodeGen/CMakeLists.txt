
add_library(obj.CodeGen OBJECT
            CodeGen.cpp
            DirectCodeGen.cpp
            GraphCodeGen.cpp

            ExprTree.cpp
            ExprTreeLifter.cpp
            IdCopier.cpp
            StackExprRemover.cpp

            TheanoEmitter.cpp
            CEmitter.cpp
            )
