//
// Created by kkeiper1103 on 5/25/2023.
//

#include <easyloggingpp/easylogging++.h>
#include <nfd.h>
#include "App.h"

App::App(const char* argv0, std::string title, int width, int height, bool fullscreen) {
    PHYSFS_init(argv0);
    PHYSFS_setSaneConfig("Predation", title.c_str(), "zip", 1, 1);

    if(!SDL_WasInit(SDL_INIT_EVERYTHING)) SDL_Init(SDL_INIT_EVERYTHING);

    NFD_Init();

    // Initialize SDL Window for Rendering
    int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
    if(fullscreen) {
        flags |= SDL_WINDOW_FULLSCREEN;

        SDL_DisplayMode dm;
        SDL_GetDesktopDisplayMode(0, &dm);

        width = dm.w;
        height = dm.h;
    }
    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);

    // initialize GL
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    gl = SDL_GL_CreateContext(window);
    gladLoadGLLoader(SDL_GL_GetProcAddress);
    SDL_GL_MakeCurrent(window, gl);

    LOG(INFO) << "GL: " << glGetString(GL_VERSION);
    LOG(INFO) << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION);

    // Initialize Nuklear
    struct nk_font_atlas* gui_font_atlas;
    ui = nk_sdl_init(window);

    nk_sdl_font_stash_begin(&gui_font_atlas);
    auto body = nk_font_atlas_add_from_file(gui_font_atlas, "resources/fonts/Arcon-Regular.otf", 20, 0);
    nk_sdl_font_stash_end();

    nk_style_set_font(ui, &body->handle);
}

App::~App() {
    nk_sdl_shutdown();
    SDL_GL_DeleteContext(gl);
    SDL_DestroyWindow(window);
    NFD_Quit();
    SDL_Quit();
    PHYSFS_deinit();
}

int App::Run() {
    Timer t;

    while(running) {
        SDL_Event e;

        nk_input_begin(ui);
        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) running = false;
            else {
                Input(e);
            }

            nk_sdl_handle_event(&e);
        }
        nk_input_end(ui);

        // process gamestate updates
        t.tick([&](float dt) {
            Update(dt);
        });

        //
        DrawUI();

        // reset GL state back to default and set viewport size
        StartFrame();

        // finally, render the application
        Render();

        // make sure the GUI gets rendered last so it overlays the application
        nk_sdl_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_MEMORY, MAX_ELEMENT_MEMORY);

        SDL_GL_SwapWindow(window);
    }

    return EXIT_SUCCESS;
}

void App::StartFrame() {
    int w, h;
    SDL_GL_GetDrawableSize(window, &w, &h);
    glViewport(0, 0, w, h);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClearDepth(100.f);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
