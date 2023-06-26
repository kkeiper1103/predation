//
// Created by kkeiper1103 on 5/13/2023.
//

#ifndef PREDATION_EDITOR_H
#define PREDATION_EDITOR_H

#include <algorithm>
#include <string>

#include "MapEditor/UI/ui.h"

#include "Common/App.h"
#include "MapEditor/Data/MapSettings.h"
#include "graphics/Mesh.h"
#include "factories/MeshFactory.h"
#include "factories/CharacterFactory.h"

class MapEditor : public App {
public:
    explicit MapEditor(const char* argv0) : App(argv0, "Map Editor", 1920, 1080, true) {}

protected:
    void Input(const SDL_Event &e) override {
        if(e.type == SDL_KEYUP && e.key.keysym.scancode == SDL_SCANCODE_ESCAPE) showQuitDialog = !showQuitDialog;
    }
    void Render() override { }
    void Update(float dt) override { }

    void DrawUI() override {
        int w, h;
        SDL_GetWindowSize(window, &w, &h);

        float paneWidth = w * .20;
        float paneOffset = w * .01;

        // @todo refactor to helper method
        if( showQuitDialog ) {

            if(nk_begin(ui, "Quit?", nk_rect(paneWidth, paneWidth, (w / 2.f) - (paneWidth / 2.f), (h / 2.f) - (paneWidth / 2.f)), NK_WINDOW_BORDER | NK_WINDOW_TITLE)) {
                nk_layout_row_dynamic(ui, 35 * scale_factor, 2);

                if(nk_button_label(ui, "No")) showQuitDialog = false;
                if(nk_button_label(ui, "Yes")) running = false;
            }
            nk_end(ui);

            return;
        }


        // left pane
        if(nk_begin(ui, "Left Pane", nk_rect(paneOffset, paneOffset, paneWidth, h - paneOffset * 2),NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_MINIMIZABLE)) {
            nk_layout_row_dynamic(ui, 35 * scale_factor, 1);

            char buffer[255];

            sprintf(buffer, "File: %s", settings.currentMap.c_str());
            nk_label(ui, buffer, NK_TEXT_ALIGN_LEFT);
            ui_file_dialog(ui, "Open Map", {{"Map", "map,MAP"}}, [&](const char* filename) {
                settings.currentMap = filename;
            });

            sprintf(buffer, "File: %s", settings.currentRsc.c_str());
            nk_label(ui, buffer, NK_TEXT_ALIGN_LEFT);
            ui_file_dialog(ui, "Open Resources File", {{"Resources", "rsc,RSC"}}, [&](const char* filename) {
                settings.currentRsc = filename;
            });

            //
            nk_label(ui, "Open CAR/3DF File", NK_TEXT_ALIGN_LEFT);
            ui_file_dialog(ui, "Open CAR/3DF File", {{"Model", "car,CAR,3df,3DF"}}, [&](const char* filename) {

                auto character = characterFactory.Get(filename);


            });

            //
            nk_label(ui, "Open All Models in Folder", NK_TEXT_ALIGN_LEFT);
            ui_folder_dialog(ui, "Select Folder", [&](const char* path) {
                fprintf(stdout, "Searching %s\n", path);
                PHYSFS_mount(path, "/models", 1);
                auto result = PHYSFS_enumerateFiles("/models");

                if (*result == NULL) {
                    fprintf(stdout, "Failed to enumerate files: %s\n", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
                }

                for(char** file = result; *file != NULL; file++) {
                    auto filename = std::string(*file);
                    // allcaps the string so we have consistent checking of file extensions (eg car == CAR == cAR etc)
                    std::transform(filename.begin(), filename.end(), filename.begin(), ::toupper);
                    if( !filename.ends_with(".CAR") && !filename.ends_with("3DF") ) {
                        continue;
                    }

                    fprintf(stdout, "Model: %s\n", *file);
                }

                PHYSFS_freeList(result);
                PHYSFS_unmount(path);
            });
        }
        nk_end(ui);

        // right pane
        if(nk_begin(ui, "Right Pane", nk_rect(w - paneWidth - (paneOffset * 2), paneOffset, paneWidth, h - paneOffset * 2), NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_MINIMIZABLE)) {
            nk_layout_row_dynamic(ui, 35 * scale_factor, 2);

            // map seed
            ui_seed_widget(ui, &settings.seed);

            //
            ui_slider_widget(ui, &settings.elevation, "Elevation Factor");

            //
            ui_slider_widget(ui, &settings.temperature, "Temperature Factor");

            //
            ui_slider_widget(ui, &settings.vegetation, "Vegetation Factor");

            //
            ui_button_widget(ui, "Update Map", []() {
                fprintf(stdout, "Update Map\n");
            });
        }
        nk_end(ui);
    }


public:
    MapSettings settings;

    MeshFactory meshFactory;
    CharacterFactory characterFactory;

    bool showQuitDialog = false;
};

#endif //PREDATION_EDITOR_H
