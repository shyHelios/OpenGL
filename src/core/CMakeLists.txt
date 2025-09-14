set(TARGET_NAME core)

set(IMGUI_SOURCES
    ${CMAKE_SOURCE_DIR}/vendor/imgui/imgui_impl_glfw.cpp
    ${CMAKE_SOURCE_DIR}/vendor/imgui/imgui_impl_opengl3.cpp
    ${CMAKE_SOURCE_DIR}/vendor/imgui/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/vendor/imgui/imgui_stdlib.cpp
    ${CMAKE_SOURCE_DIR}/vendor/imgui/imgui_tables.cpp
    ${CMAKE_SOURCE_DIR}/vendor/imgui/imgui_widgets.cpp
    ${CMAKE_SOURCE_DIR}/vendor/imgui/imgui.cpp
    ${CMAKE_SOURCE_DIR}/vendor/imgui/imgui.natvis
    ${CMAKE_SOURCE_DIR}/vendor/imgui/imgui.natstepfilter
)

set(SOURCES
    ${CMAKE_SOURCE_DIR}/src/core/vertex_array.cpp
    ${CMAKE_SOURCE_DIR}/src/core/vertex_buffer.cpp
    ${CMAKE_SOURCE_DIR}/src/core/index_buffer.cpp
    ${CMAKE_SOURCE_DIR}/src/core/shader.cpp
    ${CMAKE_SOURCE_DIR}/src/core/renderer.cpp
    ${CMAKE_SOURCE_DIR}/src/core/texture.cpp
    ${CMAKE_SOURCE_DIR}/src/core/camera.cpp
    ${CMAKE_SOURCE_DIR}/src/core/fbo.cpp
    ${CMAKE_SOURCE_DIR}/src/core/mesh.cpp
    ${CMAKE_SOURCE_DIR}/src/core/model.cpp
    ${CMAKE_SOURCE_DIR}/vendor/stb_image/stb_image.cpp
)

list(APPEND SOURCES
    ${IMGUI_SOURCES}
)

add_library(${TARGET_NAME} STATIC ${SOURCES})

target_include_directories(${TARGET_NAME} PUBLIC
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/vendor/glfw-3.4/include"
    "${CMAKE_SOURCE_DIR}/vendor/glew/include"
    "${CMAKE_SOURCE_DIR}/vendor/stb_image"
    "${CMAKE_SOURCE_DIR}/vendor/imgui"
)

target_link_directories(${TARGET_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/vendor/glew/lib/Release/x64"
    "${CMAKE_SOURCE_DIR}/vendor/glfw-3.4/lib-vc2022"
)
target_link_libraries(${TARGET_NAME} PUBLIC glm::glm assimp::assimp)
target_link_libraries(${TARGET_NAME} PRIVATE glfw3dll OpenGL32 glew32)