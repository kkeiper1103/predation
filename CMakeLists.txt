cmake_minimum_required(VERSION 3.23)
project(predation)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenGL REQUIRED)
add_subdirectory(vendor/SDL2)
add_subdirectory(vendor/physfs)

set(COMMON_SOURCES
        src/Common/Application.cpp
        src/Common/ocarn2.cpp

        src/Common/utils/Timer.cpp
        src/Common/utils/convert_rgb5_a1_to_grayscale.cpp

        src/Common/graphics/Framebuffer.cpp
        src/Common/graphics/Mesh.cpp
        src/Common/graphics/Model.cpp
        src/Common/graphics/Shader.cpp
        src/Common/graphics/Texture.cpp
        src/Common/graphics/UniformBuffer.cpp
        src/Common/graphics/Transform.h

        src/Common/collisions/BoundingBox.cpp
        src/Common/collisions/BoundingSphere.cpp

        vendor/nuklear/nuklear.c
        vendor/easyloggingpp/easylogging++.cc
        vendor/glad/src/glad.c
        )

set(GAME_SOURCES
        src/Game/Application/Kernel.cpp
        src/Game/Hunt/Hunt.cpp
        src/Game/Hunt/Terrain/SmoothedTerrain.cpp
        src/Game/Hunt/Terrain/ChunkedTerrain.cpp
        src/Game/Hunt/Camera.cpp

        src/Game/Entities/Animal.cpp
        src/Game/Entities/Hunter.cpp
        src/Game/Entities/Skybox.cpp
        src/Game/Entities/EntityMesh.cpp

        src/Game/Controls/GamepadController.cpp
        src/Game/Controls/KeyboardController.cpp

        src/Game/Hunt/Terrain/RandomTerrain.cpp
        src/Game/Hunt/Terrain/ITerrain.h
        )

set(EDITOR_SOURCES
        src/MapEditor/UI/ui.cpp src/MapEditor/UI/ui.h
        )

###
# Build Common library to be compiled / linked into the Game and MapEditor applications
#
#
###
add_library(predation-common STATIC ${COMMON_SOURCES})
add_library(Predation::Common ALIAS predation-common)
target_link_libraries(predation-common PRIVATE OpenGL::GL SDL2::SDL2 physfs)
target_include_directories(predation-common PRIVATE vendor vendor/glad/include vendor/physfs/src vendor/glm src/Common ./)


###
# Build the main Game application that replicates Carnivores 2/IceAge
#
#
###
add_executable(${PROJECT_NAME} predation.cpp ${GAME_SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL SDL2::SDL2 physfs Predation::Common)
target_include_directories(${PROJECT_NAME} PRIVATE vendor vendor/glad/include vendor/physfs/src vendor/glm src/Common src/Game ./)

###
# Build the experimental MapEditor application that generates random maps for OCARN2
#
#
###
add_executable(editor editor.cpp ${EDITOR_SOURCES})
target_link_libraries(editor PRIVATE OpenGL::GL SDL2::SDL2 physfs Predation::Common)
target_include_directories(editor PRIVATE vendor vendor/glad/include vendor/physfs/src vendor/glm src/ src/Common ./)

###
# Copies the dlls required for predation.exe into the bin dir.
#
#
###
add_custom_command(
        TARGET predation-common
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL2::SDL2>
        $<TARGET_FILE_DIR:predation-common>
        COMMENT "Copying SDL2 DLL to binary directory"
)
add_custom_command(
        TARGET predation-common
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:PhysFS::PhysFS>
        $<TARGET_FILE_DIR:predation-common>
        COMMENT "Copying PhysFS DLL to binary directory"
)

add_custom_target(copy_sdl2_dll DEPENDS Predation::Common)
add_custom_target(copy_physfs_dell DEPENDS Predation::Common)