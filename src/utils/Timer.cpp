//
// Created by kkeiper1103 on 11/23/22.
//

#include "Timer.h"

void Timer::tick(const std::function<void(double)> &tickFn) {
    double now = SDL_GetTicks64() / 1000.f;
    double delta = now - then;
    if(delta > .25)
        delta = .25;

    then = now;

    accum += delta;

    while(accum >= dt) {
        tickFn(dt);

        accum -= dt;
        t += dt;
    }
}
