cmake_minimum_required(VERSION 3.20)

project(opengl_learning DESCRIPTION "opengl learning code")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)

set(SOURCES
    src/main.cpp
    src/index_buffer.cpp
    src/vertex_buffer.cpp
)

add_executable(main ${SOURCES})
target_include_directories(main PRIVATE "vendor/glfw-3.4/include" "vendor/glew/include")
target_link_directories(main PRIVATE
    "vendor/glew/lib/Release/x64"
    "vendor/glfw-3.4/lib-vc2022"
    "D:/Windows Kits/10/Lib/10.0.22621.0/um/x64"
    COMMENT "Copying dlls into binary directory"
)
target_link_libraries(main PRIVATE glfw3dll OpenGL32 glew32)

add_custom_target(COPY_DLLS ALL COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/vendor/glfw-3.4/lib-vc2022/glfw3.dll"
    "${CMAKE_SOURCE_DIR}/vendor/glew/bin/Release/x64/glew32.dll"
    "$<TARGET_FILE_DIR:main>"
    COMMENT "Copying dlls into binary directory"
)

add_custom_target(COPY_SHADERS ALL COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/src/resources"
    "$<TARGET_FILE_DIR:main>/resources"
    COMMENT "Copying shaders into binary directory"
)