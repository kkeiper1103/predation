//
// Created by kkeiper1103 on 12/27/22.
//

#include "KeyboardController.h"

/**
 * @todo tie this into the game settings class so the controls can be rebound
 * @param action
 * @return
 */
bool KeyboardController::isActionPresent(Action action) {

    auto keys = SDL_GetKeyboardState(nullptr);

    int x{0}, y{0};
    auto mouse = SDL_GetMouseState(&x, &y);

    switch(action) {
        case Action::ACTION_FORWARD: return keys[SDL_SCANCODE_W];
            break;
        case Action::ACTION_BACKWARD: return keys[SDL_SCANCODE_S];
            break;
        case Action::ACTION_LEFT: return keys[SDL_SCANCODE_A];
            break;
        case Action::ACTION_RIGHT: return keys[SDL_SCANCODE_D];
            break;

        case Action::ACTION_JUMP: return keys[SDL_SCANCODE_SPACE];
            break;

        case Action::ACTION_TOGGLE_WEAPON: return mouse == SDL_BUTTON(SDL_BUTTON_RIGHT);
            break;
        case Action::ACTION_FIRE_WEAPON: return mouse == SDL_BUTTON(SDL_BUTTON_LEFT);
            break;

        case Action::ACTION_CHANGE_CALL: return keys[SDL_SCANCODE_V];
            break;
        case Action::ACTION_SOUND_CALL: return keys[SDL_SCANCODE_C];
            break;

        case Action::ACTION_RUN: return keys[SDL_SCANCODE_LSHIFT] | keys[SDL_SCANCODE_RSHIFT];
            break;
    }

    return false;
}
