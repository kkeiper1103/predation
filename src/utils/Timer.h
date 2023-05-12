//
// Created by kkeiper1103 on 11/23/22.
//

#ifndef BREAKOUTGL_TIMER_H
#define BREAKOUTGL_TIMER_H

#include <SDL.h>
#include <functional>

class Timer {
    double t = 0.0;
    double dt = 0.016;

    double then = SDL_GetTicks64() / 1000.f;
    double accum = 0.0;

public:
    void tick(const std::function<void(double)>& tickFn);
};


#endif //BREAKOUTGL_TIMER_H
