//
// Created by kkeiper1103 on 12/27/22.
//

#include "GamepadController.h"

#include <easyloggingpp/easylogging++.h>

bool GamepadController::isActionPresent(Action action) {
    switch(action) {
        case Action::ACTION_FORWARD: return SDL_GameControllerGetAxis(gamepad, SDL_CONTROLLER_AXIS_LEFTY) < -10000;
            break;
        case Action::ACTION_BACKWARD: return SDL_GameControllerGetAxis(gamepad, SDL_CONTROLLER_AXIS_LEFTY) > 10000;
            break;
        case Action::ACTION_LEFT: return SDL_GameControllerGetAxis(gamepad, SDL_CONTROLLER_AXIS_LEFTX) < -10000;
            break;
        case Action::ACTION_RIGHT: return SDL_GameControllerGetAxis(gamepad, SDL_CONTROLLER_AXIS_LEFTX) > 10000;
            break;
        case Action::ACTION_JUMP: return SDL_GameControllerGetButton(gamepad, SDL_CONTROLLER_BUTTON_B);
            break;
        case Action::ACTION_TOGGLE_WEAPON: return SDL_GameControllerGetButton(gamepad, SDL_CONTROLLER_BUTTON_Y);
            break;
        case Action::ACTION_FIRE_WEAPON: return SDL_GameControllerGetButton(gamepad, SDL_CONTROLLER_BUTTON_A);
            break;
        case Action::ACTION_CHANGE_CALL:
            break;
        case Action::ACTION_SOUND_CALL:
            break;
        case Action::ACTION_RUN:
            break;


        case Action::ACTION_LOOK_HORIZONTAL: return abs(SDL_GameControllerGetAxis(gamepad, SDL_CONTROLLER_AXIS_RIGHTX) ) > 10000;
            break;
        case Action::ACTION_LOOK_VERTICAL: return abs(SDL_GameControllerGetAxis(gamepad, SDL_CONTROLLER_AXIS_RIGHTY) ) > 10000;
            break;
    }

    return false;
}

float GamepadController::getActionMagnitude(Action action) {
    if(!isActionPresent(action)) return 0;


    switch(action) {
        case Action::ACTION_LOOK_VERTICAL:
            return (float) SDL_GameControllerGetAxis(gamepad, SDL_CONTROLLER_AXIS_RIGHTY) / 32767.f * 2;
            break;
        case Action::ACTION_LOOK_HORIZONTAL:
            return (float) SDL_GameControllerGetAxis(gamepad, SDL_CONTROLLER_AXIS_RIGHTX) / 32767.f * 2;
            break;
    }


    return isActionPresent(action);
}

GamepadController::GamepadController(int controllerPort) {
    gamepad = SDL_GameControllerOpen(controllerPort);

    if(gamepad == nullptr)
        LOG(ERROR) << "Unable to Open GameController #" << controllerPort;
}

GamepadController::~GamepadController() {
    SDL_GameControllerClose(gamepad);
}
