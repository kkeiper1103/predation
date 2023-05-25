//
// Created by kkeiper1103 on 12/17/22.
//

#define STB_IMAGE_IMPLEMENTATION
#include "Kernel.h"

#include <memory>

#include "Application.h"

#include <tinyfiledialogs/tinyfiledialogs.h>

Kernel::Kernel() {
    LoadGameSettings();
    LoadAssetPaths();
    LoadProfiles();

    //
    using namespace OCARN2;
    for(auto& path: assetPaths) {
        ResTxt settings = load_res_txt_file( (path + "/_RES.TXT").c_str() );


        LoadAvailableAreas(path, settings.prices.areas);
        LoadAvailableAnimals(path, settings.prices.dinos, settings.characters);
        LoadAvailableWeapons(path, settings.prices.weapons, settings.weapons);
    }

    if(profiles.empty())
        state = GS_CHOOSEPROFILE;
}

Kernel::~Kernel() {
    glDeleteTextures(1, &background.tex);

    SaveGameSettings();
    SaveAssetPaths();
    SaveProfiles();
}

void Kernel::input(SDL_Event *e) {
    if(state == GS_HUNTING) {
        // handle main input

        activeHunt->input(e);
    }

    if(e->type == SDL_KEYUP && e->key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
        if(state == GS_SETTINGS || state == GS_HUNTSETUP)
            state = GS_MAINMENU;
    }

    // handle regular input? needed with gui stuff?
    // won't anything not in hunt be gui events?
}

void Kernel::update(double dt) {
    if(state == GS_HUNTING) {
        activeHunt->update(dt);

        if(activeHunt->isComplete) {
            state = GS_MAINMENU; // @todo switch to huntsummary screen

            activeHunt.reset(nullptr); // clear out the hunt to free memory
        }
    }
}

void Kernel::gui(nk_context* ctx) {

    //
    if(background.pixels == nullptr) {
        background.pixels = stbi_load("media/scenery.png", &background.width, &background.height, &background.channels, STBI_default);
        if(background.pixels == nullptr)
            LOG(ERROR) << "Unable to Load Image ";

        auto& tex = background.tex;
        glCreateTextures(GL_TEXTURE_2D,1, &tex);
        glTextureParameterf(tex, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glTextureParameterf(tex, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glTextureParameterf(tex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameterf(tex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureStorage2D(tex, 1, GL_RGBA8, background.width, background.height);
        glTextureSubImage2D(tex, 0, 0, 0, background.width, background.height, (background.channels == 3) ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, background.pixels);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(background.pixels);
        background.image = nk_image_id((int) tex);
    }
    //



    // if the engine doesn't have any paths to carnivores 2 or ice age, prompt for path
    if( assetPaths.empty() ) {

        auto config = &app()->config;
        int w = 500, h = 200;
        if(nk_begin(ctx, "Path to HUNTDAT Folder", nk_rect(
            config->width / 2 - w / 2, config->height / 2 - h / 2,
            w, h
        ), NK_WINDOW_TITLE|NK_WINDOW_BORDER|NK_WINDOW_NO_SCROLLBAR)) {

            nk_layout_row_dynamic(ctx, 35, 1);
            if( nk_button_label(ctx, "Choose Directory") ) {
                std::vector<const char*> filters {};
                const char* path = tinyfd_selectFolderDialog("HUNTDAT Location", "");

                if(path) {
                    fprintf(stdout, "Directory: %s\n", path);
                    assetPaths.emplace_back(path);
                }
            }

            nk_layout_row_begin(ctx, NK_DYNAMIC, 35, 2);

            nk_layout_row_push(ctx, .3);
            nk_label(ctx, "HUNTDAT Location", NK_TEXT_LEFT);

            nk_layout_row_push(ctx, .7);

            static char path[255] = "";
            nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, path, 255, nk_filter_ascii);

            nk_layout_row_end(ctx);

            //
            nk_layout_row_dynamic(ctx, 35, 1);
            if(nk_button_label(ctx, "Save")) {
                if(strcmp(path, "") != 0) {
                    assetPaths.emplace_back(path);

                    SaveAssetPaths();
                }
            }
        } nk_end(ctx);

    }
    else {
        if(state == GS_MAINMENU) drawMainMenu(ctx);
        else if(state == GS_HUNTSETUP) drawHuntSetup(ctx);
        else if(state == GS_SETTINGS) drawSettings(ctx);
        else if(state == GS_CHOOSEPROFILE) drawProfileSelect(ctx);
        else if(state == GS_HUNTING) activeHunt->gui(ctx);
    }
}

void Kernel::render() {
    if(state == GS_HUNTING) {
        activeHunt->render();
    } // render world
}

void Kernel::drawMainMenu(nk_context *ctx) {
    auto config = &app()->config;



    if(nk_begin(ctx, "", nk_rect(0,0,config->width, config->height), NK_WINDOW_BACKGROUND|NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_NO_INPUT)) {

        // Draw the image
        nk_layout_row_dynamic(ctx, (float) config->height, 1);
        nk_image(ctx, background.image);
    }
    nk_end(ctx);

    if( nk_begin(ctx, "Profile Bar", nk_rect(0, 0, config->width, 50), NK_WINDOW_BORDER|NK_WINDOW_NO_SCROLLBAR) ) {

        nk_layout_row_begin(ctx, nk_layout_format::NK_DYNAMIC, 35, 5);

        nk_layout_row_push(ctx, .1);
        nk_label(ctx, "Name", NK_TEXT_LEFT);

        nk_layout_row_push(ctx, .4);
        nk_label(ctx, profiles[selectedProfile].name, NK_TEXT_LEFT);

        nk_layout_row_push(ctx, .1);
        nk_label(ctx, "Account", NK_TEXT_LEFT);

        nk_layout_row_push(ctx, .15);
        char buffer[255];
        sprintf(buffer, "%d", profiles[selectedProfile].credits);
        nk_label(ctx, buffer, NK_TEXT_LEFT);

        nk_layout_row_push(ctx, .25);
        nk_label(ctx, "Statistics", NK_TEXT_LEFT);
        // @todo create dropdown with statistics

        nk_layout_row_end(ctx);

    } nk_end(ctx);

    if(ui_panel_begin(ctx, "Predation",
                      nk_rect( 20, config->height - 20 - MENU_HEIGHT, 300, MENU_HEIGHT ), NK_WINDOW_TITLE|NK_WINDOW_BORDER|NK_WINDOW_NO_SCROLLBAR)) {

        nk_layout_row_dynamic(ctx, BUTTON_HEIGHT, 1);

        if(nk_button_label(ctx, "Hunt")) {
            state = GS_HUNTSETUP;
        }

        if(nk_button_label(ctx, "Options")) {
            state = GS_SETTINGS;
        }

        if(nk_button_label(ctx, "Trophy")) {
            // @todo add trophy mode
        }

        if(nk_button_label(ctx, "Credits")) {
            // @todo add credits
        }

        if(nk_button_label(ctx, "Switch Profile")) {
            state = GS_CHOOSEPROFILE;
        }

        if(nk_button_label(ctx, "Quit")) {
            app()->running = false;
        }

    } nk_end(ctx);
}

void Kernel::drawHuntSetup(nk_context *ctx) {
    auto config = &app()->config;

    if(nk_begin(ctx, "", nk_rect(0,0,config->width, config->height), NK_WINDOW_BACKGROUND|NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_NO_INPUT)) {

        // Draw the image
        nk_layout_row_dynamic(ctx, (float) config->height, 1);
        nk_image(ctx, background.image);
    }
    nk_end(ctx);


    if(ui_panel_begin(ctx, "Locations", nk_rect(50, 50, 300, 500), NK_WINDOW_TITLE|NK_WINDOW_BORDER|NK_WINDOW_MOVABLE)) {
        nk_layout_row_dynamic(ctx, 30, 1);

        for(auto i=0; i < areas.size(); i++) {
            auto const& entry = areas[i];

            static char buffer[255];
            sprintf(buffer, "%s: %d credits", entry.title.c_str(), entry.price);
            if(nk_option_label(ctx, buffer, huntConfig.selectedArea == i)) huntConfig.selectedArea = i;
        }
    } nk_end(ctx);

    if(ui_panel_begin(ctx, "Animals", nk_rect(400, 50, 300, 700), NK_WINDOW_TITLE|NK_WINDOW_BORDER|NK_WINDOW_MOVABLE)) {
        nk_layout_row_dynamic(ctx, 30, 1);

        for(auto i=0; i < availableAnimals.size(); i++) {
            auto const& entry = availableAnimals[i];

            static char buffer[255];
            sprintf(buffer, "%s: %d credits", entry.name.c_str(), entry.price);

            nk_checkbox_label(ctx, buffer, &huntConfig.selectedAnimals[i]);
        }
    } nk_end(ctx);

    if(ui_panel_begin(ctx, "Weapons", nk_rect(800, 50, 300, 700), NK_WINDOW_TITLE|NK_WINDOW_BORDER|NK_WINDOW_MOVABLE)) {
        nk_layout_row_dynamic(ctx, 30, 1);

        for(auto i=0; i < availableWeapons.size(); i++) {
            auto const& entry = availableWeapons[i];

            static char buffer[255];
            sprintf(buffer, "%s: %d credits", entry.name.c_str(), entry.price);

            nk_checkbox_label(ctx, buffer, &huntConfig.selectedWeapons[i]);
        }
    } nk_end(ctx);

    // time of day
    if(ui_panel_begin(ctx, "Hunt Time", nk_rect(20, 700, 300, 200), NK_WINDOW_MOVABLE|NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_TITLE|NK_WINDOW_BORDER)) {
        nk_layout_row_dynamic(ctx, 25, 1);

        if( nk_option_label(ctx, "Morning", huntConfig.timeOfDay == 0) ) huntConfig.timeOfDay = 0;
        if( nk_option_label(ctx, "Noon", huntConfig.timeOfDay == 1) ) huntConfig.timeOfDay = 1;
        if( nk_option_label(ctx, "Night", huntConfig.timeOfDay == 2) ) huntConfig.timeOfDay = 2;
    } nk_end(ctx);

    // back / hunt
    if(nk_begin(ctx, "Start Hunt", nk_rect(0, config->height - 54, config->width, 54), NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER)) {

        nk_layout_row_dynamic(ctx, BUTTON_HEIGHT, 6);

        if(nk_button_label(ctx, "<< Back")) {
            state = GS_MAINMENU;
        }

        nk_label(ctx, "", NK_TEXT_ALIGN_MIDDLE);
        nk_label(ctx, "", NK_TEXT_ALIGN_MIDDLE);
        nk_label(ctx, "", NK_TEXT_ALIGN_MIDDLE);
        nk_label(ctx, "", NK_TEXT_ALIGN_MIDDLE);

        if(nk_button_label(ctx, "Hunt >>")) {
            state = GS_HUNTING;

            // @todo
            AreaEntry area = areas[huntConfig.selectedArea];
            std::vector<AnimalEntry> animals;
            for(auto const& kv: huntConfig.selectedAnimals)
                if(kv.second == true) animals.push_back(availableAnimals[kv.first]);

            std::vector<WeaponEntry> weapons;
            for(auto const& kv: huntConfig.selectedWeapons)
                if(kv.second == true) weapons.push_back(availableWeapons[kv.first]);

            // @todo refactor to using huntConfig
            LaunchHunt( area, animals, weapons/*, perks */ );
        }

    } nk_end(ctx);
}

void Kernel::drawSettings(nk_context *ctx) {
    auto config = &app()->config;

    // game. "Accept Changes" Button lives here
    if( ui_panel_begin(ctx, "Game", nk_rect(10, 10, config->width / 2 - 5, config->height / 2 - 5), NK_WINDOW_BORDER|NK_WINDOW_TITLE|NK_WINDOW_NO_SCROLLBAR) ) {

        nk_layout_row_dynamic(ctx, 25, 2);

        nk_label(ctx, "Aggressiveness", NK_TEXT_ALIGN_RIGHT);
        nk_slider_float(ctx, 0.1, &gameSettings.aggressiveness, 2, 0.1);

        nk_label(ctx, "Density", NK_TEXT_ALIGN_RIGHT);
        nk_slider_float(ctx, 0.1, &gameSettings.density, 2, 0.1);

        nk_label(ctx, "Sensitivity", NK_TEXT_ALIGN_RIGHT);
        nk_slider_float(ctx, 0.1, &gameSettings.sensitivity, 2, 0.1);

        nk_label(ctx, "View Range", NK_TEXT_ALIGN_RIGHT);
        nk_slider_float(ctx, 0.1, &gameSettings.view_range, 2, 0.1);

        nk_label(ctx, "Measurements", NK_TEXT_ALIGN_RIGHT);

        static std::vector<std::string> measurementLabels(2);
        measurementLabels[MS_METRIC] = "Metric";
        measurementLabels[MS_IMPERIAL] = "Imperial / US";
        static int selectedMeasurements = gameSettings.measurements;

        if(nk_combo_begin_label(ctx, measurementLabels[selectedMeasurements].data(), nk_vec2(nk_widget_width(ctx), 200))) {
            nk_layout_row_dynamic(ctx, 25, 1);
            for(auto i=0; i < measurementLabels.size(); i++) {
                if(nk_combo_item_label(ctx, measurementLabels[i].data(), NK_TEXT_LEFT)) {
                    selectedMeasurements = i;
                    gameSettings.measurements = (Measurements) selectedMeasurements;
                }
            }

            nk_combo_end(ctx);
        }

        // accept changes
        if(nk_button_label(ctx, "Accept Changes")) {
            SaveGameSettings();
            state = GS_MAINMENU;
        }

    } nk_end(ctx);

    // video
    if( ui_panel_begin(ctx, "Video", nk_rect(10, config->height / 2 + 5, config->width / 2 - 5, config->height / 2 - 5), NK_WINDOW_BORDER|NK_WINDOW_TITLE|NK_WINDOW_NO_SCROLLBAR) ) {

        nk_layout_row_dynamic(ctx, 25, 1);

        nk_checkbox_label(ctx, "Fullscreen", &gameSettings.fullscreen);
        nk_checkbox_label(ctx, "3D Shadows", &gameSettings.shadows);
        nk_checkbox_label(ctx, "Fog", &gameSettings.fog);

        nk_layout_row_dynamic(ctx, 25, 2);
        nk_label(ctx, "Brightness", NK_TEXT_ALIGN_RIGHT);
        nk_slider_float(ctx, 0.1, &gameSettings.brightness, 2, 0.1);

        static std::vector<std::string> textureLabels(3);
        textureLabels[TL_AUTO] = "Auto";
        textureLabels[TL_LOW] = "Low";
        textureLabels[TL_HIGH] = "High";
        static int selectedTextureLevel = gameSettings.textureLevel;

        nk_label(ctx, "Texture Level", NK_TEXT_ALIGN_RIGHT);
        if(nk_combo_begin_label(ctx, textureLabels[selectedTextureLevel].data(), nk_vec2(nk_widget_width(ctx), 200))) {
            nk_layout_row_dynamic(ctx, 25, 1);
            for(auto i=0; i < textureLabels.size(); i++) {
                if(nk_combo_item_label(ctx, textureLabels[i].data(), NK_TEXT_LEFT)) {
                    selectedTextureLevel = i;
                    gameSettings.textureLevel = (TextureLevels) selectedTextureLevel;
                }
            }

            nk_combo_end(ctx);
        }


        nk_layout_row_dynamic(ctx, 25, 2);
        nk_label(ctx, "Field of Vision", NK_TEXT_ALIGN_RIGHT);
        nk_slider_float(ctx, 30, &gameSettings.fov, 70, 1);


        nk_layout_row_dynamic(ctx, 25, 2);
        nk_label(ctx, "View Radius", NK_TEXT_ALIGN_RIGHT);
        nk_slider_float(ctx, 64, &gameSettings.viewRadius, 256, 5);

    } nk_end(ctx);

    // controls
    if( ui_panel_begin(ctx, "Controls", nk_rect(config->width / 2 + 5, 10, config->width / 2 - 5, config->height - 10), NK_WINDOW_BORDER|NK_WINDOW_TITLE|NK_WINDOW_NO_SCROLLBAR) ) {

        nk_layout_row_begin(ctx, NK_STATIC, 25, 2);

        nk_layout_row_push(ctx, 200);
        nk_label(ctx, "Forward", NK_TEXT_ALIGN_RIGHT);
        nk_layout_row_push(ctx, 300);
        nk_label(ctx, SDL_GetScancodeName(gameSettings.controls.forward), NK_TEXT_ALIGN_LEFT);


        nk_layout_row_push(ctx, 200);
        nk_label(ctx, "Backward", NK_TEXT_ALIGN_RIGHT);
        nk_layout_row_push(ctx, 300);
        nk_label(ctx, SDL_GetScancodeName(gameSettings.controls.backward), NK_TEXT_ALIGN_LEFT);


        nk_layout_row_push(ctx, 200);
        nk_label(ctx, "Left", NK_TEXT_ALIGN_RIGHT);
        nk_layout_row_push(ctx, 300);
        nk_label(ctx, SDL_GetScancodeName(gameSettings.controls.left), NK_TEXT_ALIGN_LEFT);


        nk_layout_row_push(ctx, 200);
        nk_label(ctx, "Right", NK_TEXT_ALIGN_RIGHT);
        nk_layout_row_push(ctx, 300);
        nk_label(ctx, SDL_GetScancodeName(gameSettings.controls.right), NK_TEXT_ALIGN_LEFT);


        static std::vector<const char*> buttons {
            "No Button\0",
            "Left Mouse Button\0",
            "Middle Mouse Button\0",
            "Right Mouse Button\0"
        };

        nk_layout_row_push(ctx, 200);
        nk_label(ctx, "Fire Weapon", NK_TEXT_ALIGN_RIGHT);
        nk_layout_row_push(ctx, 300);
        nk_label(ctx, buttons[gameSettings.controls.fireWeapon], NK_TEXT_ALIGN_LEFT);


        nk_layout_row_push(ctx, 200);
        nk_label(ctx, "Toggle Weapon", NK_TEXT_ALIGN_RIGHT);
        nk_layout_row_push(ctx, 300);
        nk_label(ctx, buttons[gameSettings.controls.toggleWeapon], NK_TEXT_ALIGN_LEFT);


        nk_layout_row_push(ctx, 200);
        nk_label(ctx, "Call Animal", NK_TEXT_ALIGN_RIGHT);
        nk_layout_row_push(ctx, 300);
        nk_label(ctx, SDL_GetScancodeName(gameSettings.controls.call), NK_TEXT_ALIGN_LEFT);


        nk_layout_row_push(ctx, 200);
        nk_label(ctx, "Change Active Call", NK_TEXT_ALIGN_RIGHT);
        nk_layout_row_push(ctx, 300);
        nk_label(ctx, SDL_GetScancodeName(gameSettings.controls.changeCall), NK_TEXT_ALIGN_LEFT);


        nk_layout_row_push(ctx, 200);
        nk_label(ctx, "Binoculars", NK_TEXT_ALIGN_RIGHT);
        nk_layout_row_push(ctx, 300);
        nk_label(ctx, SDL_GetScancodeName(gameSettings.controls.binoculars), NK_TEXT_ALIGN_LEFT);

        nk_layout_row_end(ctx);

        nk_layout_row_dynamic(ctx, 25, 2);
        nk_label(ctx, "", NK_TEXT_ALIGN_RIGHT);
        nk_checkbox_label(ctx, "Invert Mouse", &gameSettings.controls.reverseMouse);

        nk_layout_row_dynamic(ctx, 25, 2);
        nk_label(ctx, "Mouse Sensitivity", NK_TEXT_ALIGN_RIGHT);
        nk_slider_float(ctx, 0.1, &gameSettings.controls.mouseSensitivity, 2, 0.1);

    } nk_end(ctx);
}

void Kernel::drawProfileSelect(nk_context *ctx) {
    auto config = &app()->config;

    int width = 300,
        height = 500;

    if(nk_begin(ctx, "", nk_rect(0,0,config->width, config->height), NK_WINDOW_BACKGROUND|NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_NO_INPUT)) {

        // Draw the image
        nk_layout_row_dynamic(ctx, (float) config->height, 1);
        nk_image(ctx, background.image);
    }
    nk_end(ctx);

    if(ui_panel_begin(ctx, "Profiles", nk_rect( config->width / 2 - width / 2, config->height / 2 - height / 2, width, height ), NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER|NK_WINDOW_TITLE)) {

        nk_layout_row_dynamic(ctx, 35, 1);

        nk_label(ctx, "Please Register:", NK_TEXT_CENTERED);

        // @todo switch to struct for new Profile
        static Profile newProfile;
        nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, newProfile.name, 256, nk_filter_ascii);

        // else list existing profiles and allow selecting of one
        static std::vector<std::string> _profiles;

        // maps the actual profiles to const char* strings for display
        std::transform(profiles.begin(), profiles.end(), std::back_inserter(_profiles), [](Profile& p) {
            return std::string(p.name) + ": " + std::to_string(p.credits) + "c";
        });

        if(nk_combo_begin_label(ctx, _profiles[selectedProfile].data(), nk_vec2(nk_widget_width(ctx), 200))) {
            nk_layout_row_dynamic(ctx, 25, 1);
            for(auto i=0; i < profiles.size(); i++) {
                if(nk_combo_item_label(ctx, _profiles[i].data(), NK_TEXT_LEFT))
                    selectedProfile = i;
            }

            nk_combo_end(ctx);
        }

        // okay and delete buttons
        nk_layout_row_dynamic(ctx, 50, 2);
        if(nk_button_label(ctx, "Okay")) {
            // if new profile isn't empty, then add to array of profiles
            if(strcmp(newProfile.name, "") != 0) {
                profiles.push_back(newProfile);
                selectedProfile = profiles.size() - 1;
            }

            SaveProfiles();

            state = GS_MAINMENU;
        }
        if(nk_button_label(ctx, "Delete")) {
            profiles.erase( profiles.begin() + selectedProfile );
            selectedProfile = 0;
        }

    } nk_end(ctx);
}

void Kernel::LoadProfiles() {
    std::fstream f("resources/profiles.dat", f.binary | f.in);

    if(!f.is_open()) {
        LOG(ERROR) << "Unable to Open Profiles File for Reading! Creating Default Profile";
        profiles.push_back({ "Jack", 100 });
        return;
    }

    int numProfiles = 0;
    f.read((char*) &numProfiles, sizeof(numProfiles));
    f.read((char*) &selectedProfile, sizeof(selectedProfile));

    profiles = std::vector<Profile>();
    for(auto i=0; i < numProfiles; i++) {
        Profile p {};
        f.read((char*) &p, sizeof(p));

        LOG(INFO) << "Reading Profile: " << p.name;

        profiles.push_back(p);
    }

    f.close();
}

void Kernel::SaveProfiles() {
    // next, write list of profiles to .dat file
    std::fstream f("resources/profiles.dat", f.binary | f.trunc | f.out);

    if(!f.is_open()) { LOG(ERROR) << "Unable to Open Profiles File for Writing!"; return; }

    int numProfiles = profiles.size();
    f.write((const char*) &numProfiles, sizeof(numProfiles));
    f.write((const char*) &selectedProfile, sizeof(selectedProfile));
    f.write((const char*) &profiles[0], profiles.size() * sizeof(Profile)); f.close();
}


void Kernel::LoadAssetPaths() {
    std::fstream f("resources/assets.txt", f.in);

    if(!f.is_open()) {
        LOG(ERROR) << "Unable to Load Asset Paths!"; return;
    }

    std::string line;
    while(!f.eof()) {
        std::getline(f, line);

        if(!line.empty()) assetPaths.emplace_back(line);
    }

    f.close();
}

void Kernel::SaveAssetPaths() {
    std::fstream f("resources/assets.txt", f.trunc | f.out);

    if(!f.is_open()) {
        LOG(ERROR) << "Unable to Save Asset Paths!"; return;
    }

    for(auto& path: assetPaths) {
        f << path << std::endl;
    }

    f.close();
}

void Kernel::LoadGameSettings() {

    std::fstream f("resources/settings.dat", f.binary | f.in);

    if(!f.is_open()) {
        LOG(ERROR) << "Unable to Read Settings File!"; return;
    }

    f.read((char*) &gameSettings, sizeof(GameSettings));

    f.close();
}

void Kernel::SaveGameSettings() {
    std::fstream f("resources/settings.dat", f.binary | f.trunc | f.out);

    if(!f.is_open()) {
        LOG(ERROR) << "Unable to Write Settings File!"; return;
    }

    f.write((char*) &gameSettings, sizeof(GameSettings));

    f.close();
}

/**
 * `areas` is a 0-based vector of prices where the index can be used to load the corresponding file
 * areas[0] corresponds to AREA1.MAP + AREA1.RSC
 * @param assetPrefix
 * @param areas
 */
void Kernel::LoadAvailableAreas(std::string &assetPrefix, const std::vector<int> &areas) {
    for(auto i=0; i < areas.size(); i++) {
        int price = areas[i];
        std::string mapname{ assetPrefix + "/AREAS/AREA" + std::to_string(i+1) + ".MAP"};
        std::string rscname{ assetPrefix + "/AREAS/AREA" + std::to_string(i+1) + ".RSC"};
        std::string descname{ assetPrefix + "/MENU/TXT/AREA" + std::to_string(i+1) + ".TXT"};

        std::string title;
        std::string description;

        // open desc file
        std::fstream f(descname, f.in);
        if(f.is_open()) {
            std::getline(f, title);
            title.erase(std::remove(title.begin(), title.end(), '\r'), title.end());

            std::string line;
            std::getline(f, line); // gets blank line

            std::stringstream ss;
            while( !f.eof() ) {
                std::getline(f, line);

                // remove the \r from the file
                line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());

                ss << line << std::endl;
            }

            description = ss.str();

            f.close();
        }

        this->areas.emplace_back(AreaEntry {price, mapname, rscname, title, description});
    }
}

/**
 * This function is similar to the above, except that we have to loop backwards through the entries,
 * since there are animals defined that cannot be used to obtain points, like Moschops, etc
 * @param prefix
 * @param animalPrices
 */
void Kernel::LoadAvailableAnimals(std::string &prefix, const std::vector<int> &animalPrices, const std::vector<OCARN2::Dino>& animals) {

    // reverse the vectors, as this will make them easier to work with since they're different sizes.
    // the animals/prices match up based on the last element in the original array, so by reversing them,
    // we're making them match from 0 (until they don't)
    std::vector<int> pricesReversed(animalPrices.size());
    std::reverse_copy(animalPrices.begin(), animalPrices.end(), pricesReversed.begin());

    std::vector<OCARN2::Dino> animalsReversed(animals.size());
    std::reverse_copy(animals.begin(), animals.end(), animalsReversed.begin());



    std::vector<AnimalEntry> tmpAnimals;
    for(auto i=0; i < animalsReversed.size(); i++) {
        if(i >= animalPrices.size()) continue;

        auto animal = animalsReversed[i];
        auto price = pricesReversed[i];


        animal.file.erase(std::remove(animal.file.begin(), animal.file.end(), '\''), animal.file.end());
        animal.name.erase(std::remove(animal.name.begin(), animal.name.end(), '\''), animal.name.end());

        // uppercase the bastard
        for(auto i=0; i < animal.file.length(); i++)
            animal.file[i] = std::toupper(animal.file[i]);

        animal.file = std::string { prefix + "/" + animal.file };

        //
        AnimalEntry entry {
            price,
            animal.name,
            animal.file
        };

        tmpAnimals.insert( tmpAnimals.begin(), entry );
    }

    //
    availableAnimals.reserve(tmpAnimals.size());
    availableAnimals.insert(availableAnimals.end(), tmpAnimals.begin(), tmpAnimals.end());

    // now updated the selectedAnimals map
    for(auto i=0; i < availableAnimals.size(); i++) {
        huntConfig.selectedAnimals[i] = false;
    }
}

/**
 * Similar logic to loading areas, except if the name matches, we don't want to add it to the available weapons, because it already has been loaded
 * carnivores 1,2,3 all use very similar weapons, with many duplicates between them
 *
 * @param prefix
 * @param prices
 * @param weapons
 */
void Kernel::LoadAvailableWeapons(std::string &prefix, std::vector<int> prices, std::vector<OCARN2::Weapon> weapons) {
    for(auto i=0; i < prices.size(); i++) {
        auto price = prices[i];
        auto weapon = weapons[i];

        weapon.file.erase(std::remove(weapon.file.begin(), weapon.file.end(), '\''), weapon.file.end());
        weapon.name.erase(std::remove(weapon.name.begin(), weapon.name.end(), '\''), weapon.name.end());
        weapon.pic.erase(std::remove(weapon.pic.begin(), weapon.pic.end(), '\''), weapon.pic.end());

        // fix casing
        for(char& i : weapon.file)
            i = std::toupper(i);

        for(char& i : weapon.pic)
            i = std::toupper(i);


        // add prefixes
        weapon.file = std::string { prefix + "/WEAPONS/" + weapon.file };
        weapon.pic = std::string { prefix + "/WEAPONS/" + weapon.pic };

        auto is_in_array = [&](const WeaponEntry& w)->bool { return w.name == weapon.name; };
        auto found = std::find_if(std::begin(availableWeapons), std::end(availableWeapons), is_in_array);

        if(found != availableWeapons.end()) {
            continue;
        }

        availableWeapons.emplace_back( WeaponEntry { price, weapon.name, weapon.file, weapon.pic } );
        huntConfig.selectedWeapons[i] = false;
    }

}



void Kernel::LaunchHunt(AreaEntry area, std::vector<AnimalEntry> animals, std::vector<WeaponEntry> weapons) {
    LOG(INFO) << area.title;
    LOG(INFO) << std::endl << area.description;

    LOG(INFO) << "Animals Selected";
    for(auto& a: animals) {
        LOG(INFO) << a.name;
    }
    LOG(INFO) << std::endl;

    LOG(INFO) << "Weapons Selected";
    for(auto& w: weapons)
        LOG(INFO) << w.name;
    LOG(INFO) << std::endl;

    // can't start hunt without an animal or gun
    if(animals.empty() || weapons.empty()) {
        state = GS_HUNTSETUP;
    }
    else {
        // all good, start hunting
        activeHunt = std::make_unique<Hunt>( this, area, animals, weapons );
    }
}

const std::vector<std::string> &Kernel::GetAssetPaths() {
    return assetPaths;
}
