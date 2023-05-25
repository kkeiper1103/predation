//
// Created by kkeiper1103 on 12/17/22.
//

#ifndef PREDATION_APPLICATION_H
#define PREDATION_APPLICATION_H

#include <random>
#include <memory>
#include <any>

#include <SDL.h>
#include <glad/glad.h>
#include <easyloggingpp/easylogging++.h>

#include "AppLogic.h"

struct ApplicationConfig {
    int width = 1600;
    int height = 1200;
    int fullscreen = 0; // `0` for none, `SDL_WINDOW_FULLSCREEN` for hardware mode, `SDL_WINDOW_FULLSCREEN_DESKTOP` for software mode

    std::string title = "Default";
};

struct Fonts {
    struct nk_font* title = nullptr;
    struct nk_font* body = nullptr;
};

class Application {
public:
    bool running = true;
    bool renderGui = true;
    SDL_Window* window = nullptr;
    SDL_GLContext glContext = nullptr;
    ApplicationConfig config {};
    struct nk_context* nkContext = nullptr;
    struct nk_font_atlas *atlas = nullptr;

    Fonts fonts;

public:
    Application();
    ~Application();

    Application& configure(ApplicationConfig config);
    void run( AppLogic* logic = nullptr );

protected:
    void render(AppLogic* logic);
};

typedef std::unique_ptr<Application> ApplicationPtr;

extern ApplicationPtr application_;
ApplicationPtr& app();
std::any app(const std::string& resolve);



inline bool ui_panel_begin(nk_context* ctx, const char* title, struct nk_rect bounds, int flags) {
    nk_style_set_font(ctx, &app()->fonts.title->handle);

    auto result = nk_begin(ctx, title, bounds, flags);

    nk_style_set_font(ctx, &app()->fonts.body->handle);

    return result;
}

#endif //PREDATION_APPLICATION_H