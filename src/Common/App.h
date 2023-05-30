//
// Created by kkeiper1103 on 5/25/2023.
//

#ifndef PREDATION_APP_H
#define PREDATION_APP_H

#include <string>

#include <SDL.h>
#include <glad/glad.h>
#include <nuklear/nuklear.h>
#include <physfs.h>

#include "utils/Timer.h"

class App {
public:
    App(const char* argv0, std::string title, int width, int height, bool fullscreen = true);
    virtual ~App();

    int Run();

protected:
    virtual void StartFrame();
    virtual void Input(const SDL_Event& e) = 0;
    virtual void Update(float delta) = 0;
    virtual void DrawUI() = 0;
    virtual void Render() = 0;

public:
    SDL_Window* window;
    SDL_GLContext gl;
    struct nk_context* ui;
    bool running = true;
};

// scaling for hdpi stuff
inline float scale_factor = 2.0f;

#endif //PREDATION_APP_H
