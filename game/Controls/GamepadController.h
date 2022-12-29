//
// Created by kkeiper1103 on 12/27/22.
//

#ifndef PREDATION_GAMEPADCONTROLLER_H
#define PREDATION_GAMEPADCONTROLLER_H

#include <SDL2/SDL.h>
#include "Controller.h"

class GamepadController : public Controller {
private:
    int gamepadId{0};
    SDL_GameController* gamepad{nullptr};

public:
    explicit GamepadController(int controllerPort = 0);
    ~GamepadController();

    bool isActionPresent(Action action) override;
    float getActionMagnitude(Action action) override;
};


#endif //PREDATION_GAMEPADCONTROLLER_H
