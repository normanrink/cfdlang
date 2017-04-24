
add_executable(test.1 test.1.cpp)
target_link_libraries(test.1 CFDlang)

add_executable(test.AxAxA test.AxAxA.cpp)
target_link_libraries(test.AxAxA CFDlang)

add_executable(test.grad test.grad.cpp)
target_link_libraries(test.grad CFDlang)

add_executable(test.Helmholtz test.Helmholtz.cpp)
target_link_libraries(test.Helmholtz CFDlang)
