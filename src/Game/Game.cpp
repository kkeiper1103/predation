//
// Created by kkeiper1103 on 5/25/2023.
//

#include "Game.h"

Game::Game(const char *argv0) : App(argv0, "Predation++", 1920, 1080, true) {
    kernel = std::make_unique<Kernel>();
}

Game::~Game() = default;

void Game::StartFrame() {
    App::StartFrame();
}

void Game::Input(const SDL_Event &e) {
    kernel->input((SDL_Event*) &e);
}

void Game::Update(float delta) {
    kernel->update(delta);
}

void Game::DrawUI() {
    kernel->gui(ui);
}

void Game::Render() {
    kernel->render();
}

