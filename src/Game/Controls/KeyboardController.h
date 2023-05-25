//
// Created by kkeiper1103 on 12/27/22.
//

#ifndef PREDATION_KEYBOARDCONTROLLER_H
#define PREDATION_KEYBOARDCONTROLLER_H

#include <SDL.h>
#include "Controller.h"

#include <map>
#include <vector>

class KeyboardController : public Controller {
protected:

public:
    bool isActionPresent(Action action) override;

    // keyboard/mouse combo is either 0 or 1, based on if the action is present
    // gamepad / joystick allows for partial values
    float getActionMagnitude(Action action) override {
        return (float) isActionPresent(action);
    }
};


#endif //PREDATION_KEYBOARDCONTROLLER_H
