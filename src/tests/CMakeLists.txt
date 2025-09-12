set(TARGET_NAME test)

add_library(${TARGET_NAME}
    STATIC
    test_clear_color.cpp
    test_texture_2d.cpp
    test_batch_rendering.cpp
    test_draw_cube.cpp
    test_material.cpp
    test_lighting_maps.cpp
    test_lights.cpp
    test.cpp
)
target_include_directories(${TARGET_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/vendor/glfw-3.4/include"
    "${CMAKE_SOURCE_DIR}/vendor/glew/include"
    "${CMAKE_SOURCE_DIR}/vendor/stb_image"
    "${CMAKE_SOURCE_DIR}/vendor/imgui")
target_link_libraries(${TARGET_NAME} PRIVATE glm::glm)