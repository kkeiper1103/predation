//
// Created by kkeiper1103 on 12/11/23.
//

#include "ComboController.h"
#include "KeyboardController.h"
#include "GamepadController.h"

bool ComboController::isActionPresent(Action action) {
    return std::any_of(controllers.begin(), controllers.end(),
                       [&](const auto& controller) { return controller->isActionPresent(action); });
}

float ComboController::getActionMagnitude(Action action) {

    for(const auto& controller: controllers) {
        if(controller->isActionPresent(action)) return controller->getActionMagnitude(action);
    }

    return 0;
}

ComboController::ComboController(int gamepadId) {

    controllers.push_back( std::make_unique<KeyboardController>() );

    if(SDL_IsGameController(gamepadId) == SDL_TRUE)
        controllers.push_back( std::make_unique<GamepadController>(gamepadId) );
}
