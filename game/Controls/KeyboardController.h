//
// Created by kkeiper1103 on 12/27/22.
//

#ifndef PREDATION_KEYBOARDCONTROLLER_H
#define PREDATION_KEYBOARDCONTROLLER_H

#include <SDL2/SDL.h>
#include "Controller.h"

#include <map>
#include <vector>

class KeyboardController : public Controller {
protected:

public:
    bool isActionPresent(Action action) override;
};


#endif //PREDATION_KEYBOARDCONTROLLER_H
