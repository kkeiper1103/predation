//
// Created by kkeiper1103 on 5/13/2023.
//

#include "editor.h"

#include <vector>

#include <easyloggingpp/easylogging++.h>
#include <physfs.h>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <glad/glad.h>
#include <nuklear/nuklear.h>
#include <cassert>


struct MapSettings {
    unsigned int seed = 0;

    float elevation = .5;
    float temperature = .5;
    float vegetation = .5;
} settings;

struct EditString {
    static constexpr int MAX_LEN = 64;
    char text[MAX_LEN] = "0";
    int len {1};
};


constexpr unsigned int FNV_PRIME = 16777619u;
constexpr unsigned int FNV_OFFSET_BASIS = 2166136261u;

unsigned int fnv1aHash(const std::string& str) {
    unsigned int hash = FNV_OFFSET_BASIS;

    for (char c : str) {
        hash ^= static_cast<unsigned int>(c);
        hash *= FNV_PRIME;
    }

    return hash;
}


char buffer[255];
void ui_slider_widget(nk_context* context, float* pValue, const char* title, float min = 0, float max = 1, float step = 0.01) {
    nk_label(context, title, NK_TEXT_ALIGN_LEFT);
    nk_slider_float(context, min, pValue, max, step);
    nk_spacer(context); sprintf(buffer, "%.2f", *pValue);
    nk_label(context, buffer, NK_TEXT_ALIGN_LEFT);
}


void ui_seed_widget(nk_context* context, unsigned int* pValue) {
    static EditString seed;
    assert(pValue != nullptr && "Pointer for Seed Buffer Can't be Null!");

    nk_label(context, "Map Seed", NK_TEXT_ALIGN_LEFT);
    nk_edit_string(context, NK_EDIT_FIELD, seed.text, &seed.len, seed.MAX_LEN, nk_filter_default);

    try {
        (*pValue) = std::stoi(seed.text);
    }
    catch(...) {
        (*pValue) = fnv1aHash(seed.text);
    }
    nk_spacer(context);
    nk_label(context, std::to_string(*pValue).c_str(), NK_TEXT_ALIGN_LEFT);
}


INITIALIZE_EASYLOGGINGPP
int main(int argc, char* argv[]) {
    START_EASYLOGGINGPP(argc, argv);

    init:
    PHYSFS_init(argv[0]);
    PHYSFS_setSaneConfig("Predation", "MapEditor", "zip", 1, 1);

    SDL_Init(SDL_INIT_EVERYTHING); std::atexit(SDL_Quit);
    SDL_Window* window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1920, 1080, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_SHOWN);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GLContext context = SDL_GL_CreateContext(window);
    gladLoadGLLoader(SDL_GL_GetProcAddress);
    SDL_GL_MakeCurrent(window, context);

    struct nk_context* gui_context;
    struct nk_font_atlas* gui_font_atlas;
    gui_context = nk_sdl_init(window);


    nk_sdl_font_stash_begin(&gui_font_atlas);
    auto body = nk_font_atlas_add_from_file(gui_font_atlas, "resources/fonts/Arcon-Regular.otf", 20, 0);
    nk_sdl_font_stash_end();

    nk_style_set_font(gui_context, &body->handle);

    loop:
    bool running = true;
    while(running) {
        SDL_Event e;

        nk_input_begin(gui_context);
        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) running = false;


            nk_sdl_handle_event(&e);
        }
        nk_input_end(gui_context);

        int w, h;
        SDL_GetWindowSize(window, &w, &h);

        static int paneWidth = 400;
        static int paneOffset = 10;

        // left pane
        if(nk_begin(gui_context, "Left Pane", nk_rect(paneOffset, paneOffset, paneWidth, h - paneOffset * 2), NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_MINIMIZABLE)) {
            nk_layout_row_dynamic(gui_context, 35, 1);


        }
        nk_end(gui_context);

        // right pane
        if(nk_begin(gui_context, "Right Pane", nk_rect(w - paneWidth - (paneOffset * 2), paneOffset, paneWidth, h - paneOffset * 2), NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_MINIMIZABLE)) {
            nk_layout_row_dynamic(gui_context, 35, 2);



            // map seed
            ui_seed_widget(gui_context, &settings.seed);

            //
            ui_slider_widget(gui_context, &settings.elevation, "Elevation Factor");

            //
            ui_slider_widget(gui_context, &settings.temperature, "Temperature Factor");

            //
            ui_slider_widget(gui_context, &settings.vegetation, "Vegetation Factor");


            //
            nk_layout_row_dynamic(gui_context, 35, 1);
            if(nk_button_label(gui_context, "Update Map")) {
                fprintf(stdout, "Updating Map...\n");
            }
        }
        nk_end(gui_context);

        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        // render map

        nk_sdl_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_MEMORY, MAX_ELEMENT_MEMORY);

        SDL_GL_SwapWindow(window);
    }


    quit:
    nk_sdl_shutdown();
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    PHYSFS_deinit();

    return 0;
}