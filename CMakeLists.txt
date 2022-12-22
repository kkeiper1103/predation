cmake_minimum_required(VERSION 3.23)
project(predation)

set(CMAKE_CXX_STANDARD 20)


find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)

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

        game/World/GameWorld.cpp
        game/Hunt/Hunt.cpp
        game/Hunt/Terrain/ChunkedTerrain.cpp

        game/Entities/Animal.cpp
        game/Entities/Hunter.cpp
        game/Entities/Skybox.cpp
        game/Entities/EntityMesh.cpp


        vendor/nuklear/nuklear.c
        vendor/easyloggingpp/easylogging++.cc
        vendor/glad/src/glad.c
        )

add_executable(${PROJECT_NAME} main.cpp ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL SDL2::SDL2)
target_include_directories(${PROJECT_NAME} PRIVATE vendor vendor/glad/include vendor/glm src/ game/)



# render tests
add_executable(testrunner tests/rendermodel.cpp ${SOURCES})
target_link_libraries(testrunner PRIVATE OpenGL::GL SDL2::SDL2)
target_include_directories(testrunner PRIVATE vendor vendor/glad/include vendor/glm src/ game/)