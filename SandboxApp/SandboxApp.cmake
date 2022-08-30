set(SOURCES_SANDBOXAPP
              SandboxApp/src/SandboxApp.cpp
                    )

add_executable(SandboxApp ${SOURCES_SANDBOXAPP})

target_link_libraries(SandboxApp Xenode)
target_include_directories(SandboxApp PUBLIC Xenode/src)
