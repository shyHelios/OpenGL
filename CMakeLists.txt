cmake_minimum_required(VERSION 3.20)

project(opengl_learning DESCRIPTION "opengl learning code")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)

set(glm_DIR "E:/Code/Library/glm/share/glm")
find_package(glm REQUIRED CONFIG)

add_subdirectory(vendor/assimp)

add_subdirectory(src/core)
add_subdirectory(src/tests)

add_executable(app src/application.cpp)
target_link_libraries(app PRIVATE core test glfw3dll)
target_link_directories(app PRIVATE
    "${CMAKE_SOURCE_DIR}/vendor/glew/lib/Release/x64"
    "${CMAKE_SOURCE_DIR}/vendor/glfw-3.4/lib-vc2022"
)

add_custom_target(COPY_DLLS ALL COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/vendor/glfw-3.4/lib-vc2022/glfw3.dll"
    "${CMAKE_SOURCE_DIR}/vendor/glew/bin/Release/x64/glew32.dll"
    "$<TARGET_FILE_DIR:app>"
    COMMENT "Copying dlls into binary directory"
)

add_custom_target(COPY_RESOURCES ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:app>/resources"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/resources" "$<TARGET_FILE_DIR:app>/resources"
    COMMENT "Copying resources into binary directory"
)