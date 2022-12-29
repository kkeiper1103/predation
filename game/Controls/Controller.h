//
// Created by kkeiper1103 on 12/27/22.
//

#ifndef PREDATION_CONTROLLER_H
#define PREDATION_CONTROLLER_H

enum class Action {
    ACTION_FORWARD,
    ACTION_BACKWARD,
    ACTION_LEFT,
    ACTION_RIGHT,
    ACTION_JUMP,
    ACTION_TOGGLE_WEAPON,
    ACTION_FIRE_WEAPON,

    ACTION_CHANGE_CALL,
    ACTION_SOUND_CALL,
    ACTION_RUN
};

struct Controller {
    virtual bool isActionPresent(Action action) = 0;
};

#endif //PREDATION_CONTROLLER_H
