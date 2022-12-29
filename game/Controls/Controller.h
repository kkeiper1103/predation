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

    // these are primarily for gamepads, but we might want to find a way to map these to mouse movement for a unified api
    ACTION_LOOK_HORIZONTAL,
    ACTION_LOOK_VERTICAL,

    ACTION_JUMP,
    ACTION_TOGGLE_WEAPON,
    ACTION_FIRE_WEAPON,

    ACTION_CHANGE_CALL,
    ACTION_SOUND_CALL,
    ACTION_RUN
};

struct Controller {
    virtual bool isActionPresent(Action action) = 0;
    virtual float getActionMagnitude(Action action) = 0;
};

#endif //PREDATION_CONTROLLER_H
