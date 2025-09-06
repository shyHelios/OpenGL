cmake_minimum_required(VERSION 3.20)

project(opengl_learning DESCRIPTION "opengl learning code")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)

set(glm_DIR "E:/Code/Library/glm/share/glm")
find_package(glm REQUIRED CONFIG)

set(IMGUI_SOURCES
    vendor/imgui/imgui_impl_glfw.cpp
    vendor/imgui/imgui_impl_opengl3.cpp
    vendor/imgui/imgui_draw.cpp
    vendor/imgui/imgui_stdlib.cpp
    vendor/imgui/imgui_tables.cpp
    vendor/imgui/imgui_widgets.cpp
    vendor/imgui/imgui.cpp
    vendor/imgui/imgui.natvis
    vendor/imgui/imgui.natstepfilter
)

set(SOURCES
    src/application.cpp
    src/vertex_array.cpp
    src/vertex_buffer.cpp
    src/index_buffer.cpp
    src/shader.cpp
    src/renderer.cpp
    src/texture.cpp
    src/batch_renderer.cpp
    src/camera.cpp
    vendor/stb_image/stb_image.cpp
)

add_subdirectory(src/tests)

list(APPEND SOURCES
    ${IMGUI_SOURCES}
)

add_executable(main ${SOURCES})
target_include_directories(main PRIVATE
    "${CMAKE_SOURCE_DIR}/vendor/glfw-3.4/include"
    "${CMAKE_SOURCE_DIR}/vendor/glew/include"
    "${CMAKE_SOURCE_DIR}/vendor/stb_image"
    "${CMAKE_SOURCE_DIR}/vendor/imgui")
target_link_libraries(main PRIVATE glm::glm test)

target_link_directories(main PRIVATE
    "${CMAKE_SOURCE_DIR}/vendor/glew/lib/Release/x64"
    "${CMAKE_SOURCE_DIR}/vendor/glfw-3.4/lib-vc2022"
    "${CMAKE_SOURCE_DIR}/D:/Windows Kits/10/Lib/10.0.22621.0/um/x64"
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

add_custom_target(COPY_FONT ALL COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/vendor/imgui/msyh.ttf"
    "$<TARGET_FILE_DIR:main>/resources"
    COMMENT "Copying font into binary directory"
)