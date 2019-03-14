
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11")

add_executable(test.1 test.1.cpp)
target_link_libraries(test.1 CFDlang)

add_executable(test.AxAxA test.AxAxA.cpp)
target_link_libraries(test.AxAxA CFDlang)

add_executable(test.grad test.grad.cpp)
target_link_libraries(test.grad CFDlang)

add_executable(test.Helmholtz test.Helmholtz.cpp)
target_link_libraries(test.Helmholtz CFDlang)

add_executable(test.Operators.Print test.Operators.Print.cpp)
target_link_libraries(test.Operators.Print CFDlang)

add_executable(test.Operators.Op_2D test.Operators.Op_2D.cpp)
target_link_libraries(test.Operators.Op_2D CFDlang)

add_executable(test.Operators.Op_3D test.Operators.Op_3D.cpp)
target_link_libraries(test.Operators.Op_3D CFDlang)

add_executable(test.Operators.AxAxA test.Operators.AxAxA.cpp)
target_link_libraries(test.Operators.AxAxA CFDlang)
