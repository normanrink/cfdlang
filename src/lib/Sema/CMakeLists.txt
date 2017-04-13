
add_library(Sema STATIC
            Sema.cpp
            Symbol.cpp
            TensorType.cpp)

add_library(obj.Sema OBJECT
            Sema.cpp
            Symbol.cpp
            TensorType.cpp)

