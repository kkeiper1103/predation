//
// Created by kkeiper1103 on 5/13/2023.
//

#define SDL_MAIN_HANDLED
#include "editor.h"

#include <easyloggingpp/easylogging++.h>

INITIALIZE_EASYLOGGINGPP
int main(int argc, char* argv[]) {
    START_EASYLOGGINGPP(argc, argv);

    MapEditor editor(argv[0]);
    return editor.Run();
}