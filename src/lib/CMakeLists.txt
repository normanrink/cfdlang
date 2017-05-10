
add_subdirectory(AST)
add_subdirectory(CodeGen)
add_subdirectory(Parse)
add_subdirectory(Runtime)
add_subdirectory(Sema)

add_library(CFDlang $<TARGET_OBJECTS:obj.AST>
                    $<TARGET_OBJECTS:obj.CodeGen>
                    $<TARGET_OBJECTS:obj.Parser>
                    $<TARGET_OBJECTS:obj.Runtime>
                    $<TARGET_OBJECTS:obj.Sema>)
target_link_libraries(CFDlang dl)

