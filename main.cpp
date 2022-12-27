#include <iostream>

#include <easyloggingpp/easylogging++.h>
INITIALIZE_EASYLOGGINGPP


#include "Application.h"
#include "World/GameWorld.h"

int main(int argc, char* argv[]) {
    START_EASYLOGGINGPP(argc, argv);

    GameWorld logic;

    app()->configure({
        1600,
        900,
        0,
        "Predation++: Carnivores Clone"
    }).run(&logic);

    return 0;
}
