#include <iostream>

#include <easyloggingpp/easylogging++.h>
INITIALIZE_EASYLOGGINGPP


#include "Application.h"
#include "World/GameWorld.h"


int main(int argc, char* argv[]) {
    START_EASYLOGGINGPP(argc, argv);

    GameWorld logic;

    app()->configure({
        1920,
        1080,
        SDL_WINDOW_FULLSCREEN_DESKTOP,
        "Predation++: Carnivores Clone"
    }).run(&logic);

    return 0;
}
