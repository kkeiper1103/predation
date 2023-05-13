#include <iostream>

#include "predation.h"

#include <easyloggingpp/easylogging++.h>
INITIALIZE_EASYLOGGINGPP


#define SDL_MAIN_HANDLED
#include "Application.h"
#include "Application/Kernel.h"


int main(int argc, char* argv[]) {
    START_EASYLOGGINGPP(argc, argv);

    Kernel logic;

    app()->configure({
        1920,
        1080,
        SDL_WINDOW_FULLSCREEN_DESKTOP,
        "Predation++: Carnivores Clone"
    }).run(&logic);

    return 0;
}
