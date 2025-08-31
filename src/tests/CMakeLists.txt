set(TARGET_NAME test)

add_library(${TARGET_NAME}
    STATIC
    test_clear_color.cpp
)
target_include_directories(${TARGET_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/vendor/glfw-3.4/include"
    "${CMAKE_SOURCE_DIR}/vendor/glew/include"
    "${CMAKE_SOURCE_DIR}/vendor/stb_image"
    "${CMAKE_SOURCE_DIR}/vendor/imgui")