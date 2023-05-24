#include <iostream>

#include "predation.h"

#include <easyloggingpp/easylogging++.h>
INITIALIZE_EASYLOGGINGPP


#define SDL_MAIN_HANDLED
#include "Application.h"
#include "Application/Kernel.h"

#include <physfs.h>

int main(int argc, char* argv[]) {
    START_EASYLOGGINGPP(argc, argv);

    Kernel logic;

    PHYSFS_init(argv[0]);
    PHYSFS_setSaneConfig("Predation", "Predation++", "zip", 1, 1);

    app()->configure({
        1920,
        1080,
        SDL_WINDOW_FULLSCREEN_DESKTOP,
        "Predation++: Carnivores Clone"
    }).run(&logic);

    PHYSFS_deinit();

    return 0;
}
