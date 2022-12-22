#include <iostream>
#include <memory>

#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <easyloggingpp/easylogging++.h>
INITIALIZE_EASYLOGGINGPP

#include <ocarn2.h>

#include "Entities/Skybox.h"
#include "graphics/Mesh.h"
#include "graphics/Model.h"

//
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#define width 800.f
#define height 600.f

SDL_Window* window = nullptr;
SDL_GLContext context = nullptr;

void clearWindow();

int main(int argc, char* argv[]) {
    START_EASYLOGGINGPP(argc, argv);

    SDL_Init(SDL_INIT_VIDEO);
    atexit(SDL_Quit);

    window = SDL_CreateWindow("Testrunner", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);


    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);

    context = SDL_GL_CreateContext(window);
    if(!context)
        printf("Unable to Create glContext\n");

    SDL_GL_MakeCurrent(window, context);
    SDL_GL_SetSwapInterval(1);

   if(! gladLoadGLLoader(SDL_GL_GetProcAddress) ) {
       printf("Unable to install Glad Loader\n");
   }


    // setup active test
    glm::mat4 view = glm::lookAt(glm::vec3{0, 0, 0}, glm::vec3{0, 0, -1}, glm::vec3{0, 1, 0});
    glm::mat4 projection = glm::perspective(glm::radians(45.f), (float) width / (float) height, .1f, 100.f);
    glm::mat4 model(1.f);

    auto data = load_rsc_file("/home/kkeiper1103/.wine/drive_c/Program Files (x86)/Carnivores 2/HUNTDAT/AREAS/AREA1.RSC");
    std::unique_ptr<Skybox> skybox = std::make_unique<Skybox>(data.sky[0]);


    std::vector<Vertex> vertices {
        {0.5f, 0.5f, 0.0f, 1.f, 1.f},  // top right
        {0.5f, -0.5f, 0.0f, 1.f, 0.f},  // bottom right
        {-0.5f, -0.5f, 0.0f, 0.f, 0.f},  // bottom left
        {-0.5f, 0.5f, 0.0f, 0.f, 1.f}   // top left
    };
    std::vector<Mesh::idx_t> indices {
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };

    auto mesh = std::make_shared<Mesh>(vertices, indices);
    auto instance = std::make_shared<Model>(mesh, glm::vec3 {0, 0, -10});

    mesh->shader->setMat4("view", view);
    mesh->shader->setMat4("projection", projection);







    bool running = true;
    while(running) {
        SDL_Event e;
        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) running = false;
        }

        skybox->update(0.016);

        clearWindow();

        // runActiveTest();

        skybox->shader->use();

        skybox->shader->setMat4("projection", projection);
        skybox->shader->setMat4("view", view);
        skybox->shader->setMat4("model", glm::mat4(1.f));

        instance->draw();

        SDL_GL_SwapWindow(window);
    }

    free_rsc(data);

    SDL_DestroyWindow(window);
    SDL_GL_DeleteContext(context);
}


void clearWindow() {

    glViewport(0, 0, width, height);
    glEnable(GL_BLEND | GL_DEPTH_TEST | GL_MULTISAMPLE);
    glDepthFunc(GL_LEQUAL);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    glClearColor(.5, .5, .5, .5);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}