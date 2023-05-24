cmake_minimum_required(VERSION 3.23)
project(predation)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenGL REQUIRED)
add_subdirectory(vendor/SDL2)
add_subdirectory(vendor/physfs)

set(SOURCES

        src/Application.cpp
        src/ocarn2.cpp

        src/utils/Timer.cpp
        src/utils/convert_rgb5_a1_to_grayscale.cpp

        src/graphics/Framebuffer.cpp
        src/graphics/Mesh.cpp
        src/graphics/Model.cpp
        src/graphics/Shader.cpp
        src/graphics/Texture.cpp
        src/graphics/UniformBuffer.cpp
        src/graphics/Transform.h

        src/collisions/BoundingBox.cpp
        src/collisions/BoundingSphere.cpp

        game/Application/Kernel.cpp
        game/Hunt/Hunt.cpp
        game/Hunt/Terrain/ChunkedTerrain.cpp
        game/Hunt/Terrain/SmoothedTerrain.cpp
        game/Hunt/Camera.cpp

        game/Entities/Animal.cpp
        game/Entities/Hunter.cpp
        game/Entities/Skybox.cpp
        game/Entities/EntityMesh.cpp

        game/Controls/KeyboardController.cpp
        game/Controls/GamepadController.cpp

        game/Hunt/Terrain/RandomTerrain.cpp
        game/Hunt/Terrain/ITerrain.h

        vendor/nuklear/nuklear.c
        vendor/easyloggingpp/easylogging++.cc
        vendor/glad/src/glad.c
        )

add_executable(${PROJECT_NAME} predation.cpp ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL SDL2::SDL2 physfs)
target_include_directories(${PROJECT_NAME} PRIVATE vendor vendor/glad/include vendor/physfs/src vendor/glm src/ game/ ./)


add_executable(editor editor.cpp ${SOURCES})
target_link_libraries(editor PRIVATE OpenGL::GL SDL2::SDL2)
target_include_directories(editor PRIVATE vendor vendor/glad/include vendor/glm src/ game/ ./)


# render tests
add_executable(testrunner tests/rendermodel.cpp ${SOURCES})
target_link_libraries(testrunner PRIVATE OpenGL::GL SDL2::SDL2)
target_include_directories(testrunner PRIVATE vendor vendor/glad/include vendor/glm src/ game/)

add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL2::SDL2>
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMENT "Copying SDL2 DLL to binary directory"
)
add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:PhysFS::PhysFS>
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMENT "Copying PhysFS DLL to binary directory"
)

add_custom_target(copy_sdl2_dll DEPENDS ${PROJECT_NAME})
add_custom_target(copy_physfs_dell DEPENDS ${PROJECT_NAME})