
add_executable(addition addition.cpp)
target_link_libraries(addition CFDlang)

add_executable(contraction contraction.cpp)
target_link_libraries(contraction CFDlang)

add_executable(AxAxA AxAxA.cpp)
target_link_libraries(AxAxA CFDlang)

add_executable(addition.elem-naive addition.elem-naive.cpp)
target_link_libraries(addition.elem-naive CFDlang)

add_executable(addition.elem addition.elem.cpp)
target_compile_options(addition.elem PRIVATE "-fopenmp")
target_link_options(addition.elem PRIVATE "-fopenmp")
target_link_libraries(addition.elem CFDlang)
