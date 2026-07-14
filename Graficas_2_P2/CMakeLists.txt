#all external Files in the assets Dir
file(GLOB_RECURSE ASSET_FILES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/assets/*")

# Add source to this project's executable.
add_executable (Graficas_2_P2 "src/Graficas_2_P2.cpp" "src/Graficas_2_P2.h"
                              "src/Shader.h" "src/Shader.cpp"
                              "src/BuffersManagement/VBO.h" "src/BuffersManagement/VBO.cpp"
                              "src/BuffersManagement/VAO.h" "src/BuffersManagement/VAO.cpp"
                              "src/BuffersManagement/EBO.h" "src/BuffersManagement/EBO.cpp"
                              "src/CameraManagement/Camera.h" "src/CameraManagement/Camera.cpp"
                              "src/ModelManagement/volumenTexture.h" "src/ModelManagement/volumenTexture.cpp"
                              "src/ElementManagement/Quad.h" "src/ElementManagement/Quad.cpp"
                              "src/ElementManagement/Gizmos.h" "src/ElementManagement/Gizmos.cpp"
                              ${ASSET_FILES}
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Graficas_2_P2 PROPERTY CXX_STANDARD 20)
endif()

add_custom_target(SyncAssets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "${CMAKE_CURRENT_BINARY_DIR}/assets"
)

add_dependencies(Graficas_2_P2 SyncAssets)

target_link_libraries(Graficas_2_P2 PRIVATE
	glfw
    glm
    glad
    stb
    gltf
    imgui
    tinyobjloader
    fastnoiselite
    OpenGL::GL
)
