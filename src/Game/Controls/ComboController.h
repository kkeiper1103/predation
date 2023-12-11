//
// Created by kkeiper1103 on 12/11/23.
//

#pragma once

#include <memory>
#include <vector>

#include "Controller.h"

class ComboController : public Controller {
public:
    explicit ComboController(int gamepadId = 0);

    bool isActionPresent(Action action) override;
    float getActionMagnitude(Action action) override;

protected:
    std::vector< std::unique_ptr<Controller> > controllers;
};
