//
// Created by kkeiper1103 on 5/13/2023.
//

#ifndef PREDATION_EDITOR_H
#define PREDATION_EDITOR_H

#include "MapEditor/UI/ui.h"

#include "Common/App.h"
#include "MapEditor/Data/MapSettings.h"
#include "graphics/Mesh.h"

class MapEditor : public App {
public:
    explicit MapEditor(const char* argv0) : App(argv0, "Map Editor", 1920, 1080, true) {}

protected:
    void Input(const SDL_Event &e) override { }
    void Render() override { }
    void Update(float dt) override { }

    void DrawUI() override {
        int w, h;
        SDL_GetWindowSize(window, &w, &h);

        static int paneWidth = 400;
        static int paneOffset = 10;

        // left pane
        if(nk_begin(ui, "Left Pane", nk_rect(paneOffset, paneOffset, paneWidth, h - paneOffset * 2),NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_MINIMIZABLE)) {
            nk_layout_row_dynamic(ui, 35, 1);

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
                Mesh mesh(filename);

                mesh.Debug();
            });

            //
            nk_label(ui, "Open All Models in Folder", NK_TEXT_ALIGN_LEFT);
            ui_folder_dialog(ui, "Select Folder", [&](const char* path) {
                fprintf(stdout, "Searching %s\n", path);
                PHYSFS_mount(path, NULL, 1);
                auto result = PHYSFS_enumerateFiles(path);

                if (*result == NULL) {
                    fprintf(stdout, "Failed to enumerate files: %s\n", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
                }

                for(char** file = result; *file != NULL; file++) {
                    fprintf(stdout, "Model: %s\n", *file);
                }

                PHYSFS_freeList(result);
                PHYSFS_unmount(path);
            });
        }
        nk_end(ui);

        // right pane
        if(nk_begin(ui, "Right Pane", nk_rect(w - paneWidth - (paneOffset * 2), paneOffset, paneWidth, h - paneOffset * 2), NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_MINIMIZABLE)) {
            nk_layout_row_dynamic(ui, 35, 2);

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
};

#endif //PREDATION_EDITOR_H
