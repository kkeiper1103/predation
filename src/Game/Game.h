//
// Created by kkeiper1103 on 5/25/2023.
//

#ifndef PREDATION_GAME_H
#define PREDATION_GAME_H


#include <memory>
#include "App.h"
#include "Application/Kernel.h"

class Game : public App {
public:
    explicit Game(const char* argv0);
    ~Game() override;

protected:
    void StartFrame() override;
    void Input(const SDL_Event &e) override;
    void Update(float delta) override;
    void DrawUI() override;
    void Render() override;

protected:
    std::unique_ptr<Kernel> kernel;
};


#endif //PREDATION_GAME_H
