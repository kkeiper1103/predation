//
// Created by kkeiper1103 on 12/17/22.
//

#ifndef PREDATION_APPLOGIC_H
#define PREDATION_APPLOGIC_H

#include <SDL2/SDL_events.h>
#include <nuklear/nuklear.h>

class AppLogic {
public:
    virtual void input(SDL_Event* e) = 0;
    virtual void update(double dt) = 0;
    virtual void gui(nk_context*) = 0;
    virtual void render() = 0;
};

#endif //PREDATION_APPLOGIC_H
