//
// Created by kkeiper1103 on 12/17/22.
//

#ifndef PREDATION_GAMESETTINGS_H
#define PREDATION_GAMESETTINGS_H

#include <SDL2/SDL.h>

enum Measurements {
    MS_METRIC = 0,
    MS_IMPERIAL
};

enum TextureLevels {
    TL_AUTO = 0,
    TL_LOW,
    TL_HIGH
};

struct KeyBindings {
    SDL_Scancode forward{SDL_SCANCODE_W};
    SDL_Scancode backward{SDL_SCANCODE_S};
    SDL_Scancode left{SDL_SCANCODE_A};
    SDL_Scancode right{SDL_SCANCODE_D};
    SDL_Scancode jump{SDL_SCANCODE_SPACE};
    SDL_Scancode run{SDL_SCANCODE_LSHIFT};

    int fireWeapon = SDL_BUTTON_LEFT;
    int toggleWeapon = SDL_BUTTON_RIGHT;

    SDL_Scancode call{SDL_SCANCODE_LCTRL};
    SDL_Scancode changeCall{SDL_SCANCODE_C};
    SDL_Scancode binoculars{SDL_SCANCODE_B};

    int reverseMouse = false;
    float mouseSensitivity = 1.f;
};

struct GameSettings {

    // dinosaur settings
    float aggressiveness = 1.0;
    float density = 1.0;
    float sensitivity = 1.f;
    float view_range = 1.f;
    Measurements measurements = MS_METRIC;

    // video & audio
    // @todo list resolutions
    int fullscreen = true;
    int shadows = true;
    int fog = true;
    TextureLevels textureLevel = TL_AUTO;
    float brightness = 1.f;
    float fov = 45.f;

    // controls
    KeyBindings controls;
};

#endif //PREDATION_GAMESETTINGS_H
