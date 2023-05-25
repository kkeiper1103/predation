//
// Created by kkeiper1103 on 5/13/2023.
//

#include "editor.h"

#include <vector>
#include <cassert>

#include <easyloggingpp/easylogging++.h>
#include <physfs.h>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <glad/glad.h>
#include <nuklear/nuklear.h>

#include "MapEditor/UI/ui.h"

struct MapSettings {
    unsigned int seed = 0;

    std::string currentMap;
    std::string currentRsc;

    float elevation = .5;
    float temperature = .5;
    float vegetation = .5;
} settings;

/**
 *
 * @param window
 * @param gl
 * @param ui
 * @param argv0
 */
void init(SDL_Window** window, SDL_GLContext* gl, nk_context** ui, const char* argv0) {

    // setup file system abstraction
    PHYSFS_init(argv0);
    PHYSFS_setSaneConfig("Predation", "MapEditor", "zip", 1, 1);

    // start sdl and create window
    SDL_Init(SDL_INIT_EVERYTHING); std::atexit(SDL_Quit);
    *window = SDL_CreateWindow("MapEditor", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1920, 1080, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_SHOWN);

    // initialize opengl 46 and use glad to load functions
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    *gl = SDL_GL_CreateContext(*window);
    gladLoadGLLoader(SDL_GL_GetProcAddress);
    SDL_GL_MakeCurrent(*window, *gl);

    // initialize nuklear gui settings here
    struct nk_font_atlas* gui_font_atlas;
    *ui = nk_sdl_init(*window);

    nk_sdl_font_stash_begin(&gui_font_atlas);
    auto body = nk_font_atlas_add_from_file(gui_font_atlas, "resources/fonts/Arcon-Regular.otf", 20, 0);
    nk_sdl_font_stash_end();

    nk_style_set_font(*ui, &body->handle);
}

/**
 *
 * @param window
 * @param glContext
 */
void quit(SDL_Window** window, SDL_GLContext* glContext) {
    nk_sdl_shutdown();
    SDL_GL_DeleteContext(*glContext);
    SDL_DestroyWindow(*window);
    PHYSFS_deinit();
}

INITIALIZE_EASYLOGGINGPP
int main(int argc, char* argv[]) {
    START_EASYLOGGINGPP(argc, argv);

    SDL_Window* window;
    SDL_GLContext glContext;
    struct nk_context* nkContext;

    init(&window, &glContext, &nkContext, argv[0]);

    loop:
    bool running = true;
    while(running) {
        SDL_Event e;

        nk_input_begin(nkContext);
        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) running = false;


            nk_sdl_handle_event(&e);
        }
        nk_input_end(nkContext);

        int w, h;
        SDL_GetWindowSize(window, &w, &h);

        static int paneWidth = 400;
        static int paneOffset = 10;

        // left pane
        if(nk_begin(nkContext, "Left Pane", nk_rect(paneOffset, paneOffset, paneWidth, h - paneOffset * 2),NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_MINIMIZABLE)) {
            nk_layout_row_dynamic(nkContext, 35, 1);

            char buffer[255];

            sprintf(buffer, "File: %s", settings.currentMap.c_str());
            nk_label(nkContext, buffer, NK_TEXT_ALIGN_LEFT);
            ui_file_dialog(nkContext, "Open Map", {"*.map", "*.MAP"}, [&](const char* filename) {
                settings.currentMap = filename;
            });

            sprintf(buffer, "File: %s", settings.currentRsc.c_str());
            nk_label(nkContext, buffer, NK_TEXT_ALIGN_LEFT);
            ui_file_dialog(nkContext, "Open Resources File", {"*.rsc", "*.RSC"}, [&](const char* filename) {
                settings.currentRsc = filename;
            });
        }
        nk_end(nkContext);

        // right pane
        if(nk_begin(nkContext, "Right Pane", nk_rect(w - paneWidth - (paneOffset * 2), paneOffset, paneWidth, h - paneOffset * 2), NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_MINIMIZABLE)) {
            nk_layout_row_dynamic(nkContext, 35, 2);

            // map seed
            ui_seed_widget(nkContext, &settings.seed);

            //
            ui_slider_widget(nkContext, &settings.elevation, "Elevation Factor");

            //
            ui_slider_widget(nkContext, &settings.temperature, "Temperature Factor");

            //
            ui_slider_widget(nkContext, &settings.vegetation, "Vegetation Factor");

            //
            ui_button_widget(nkContext, "Update Map", []() {
                fprintf(stdout, "Update Map\n");
            });
        }
        nk_end(nkContext);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glClearDepth(100.f);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // render map

        nk_sdl_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_MEMORY, MAX_ELEMENT_MEMORY);

        SDL_GL_SwapWindow(window);
    }

    quit(&window, &glContext);

    return 0;
}