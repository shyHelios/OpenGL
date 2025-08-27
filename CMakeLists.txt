cmake_minimum_required(VERSION 3.20)

project(opengl_learning DESCRIPTION "opengl learning code")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR})

set(SOURCES
    main.cpp
)

add_executable(main ${SOURCES})
target_include_directories(main PRIVATE "E:/Code/Library/glfw-3.4/include")
target_link_directories(main PRIVATE "E:/Code/Library/glfw-3.4/lib-vc2022" "D:/Windows Kits/10/Lib/10.0.22621.0/um/x64")
target_link_libraries(main PRIVATE glfw3dll OpenGL32)

add_custom_target(COPY_DLLS ALL COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "E:/Code/Library/glfw-3.4/lib-vc2022/glfw3.dll"
    "$<TARGET_FILE_DIR:main>"
)