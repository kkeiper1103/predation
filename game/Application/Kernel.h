//
// Created by kkeiper1103 on 12/17/22.
//

#ifndef PREDATION_KERNEL_H
#define PREDATION_KERNEL_H

#include <string>
#include <map>

#include <stb/stb_image.h>

#include "HuntConfig.h"
#include "GameSettings.h"
#include "AppLogic.h"
#include "Profile.h"
#include "AreaEntry.h"

#include "ocarn2.h"
#include "AnimalEntry.h"
#include "WeaponEntry.h"
#include "Hunt/Hunt.h"

#include "predation.h"

enum GameState {
    GS_CHOOSEPROFILE,
    GS_MAINMENU,
    GS_HUNTSETUP,
    GS_HUNTING,
    GS_HUNTRESULTS,
    GS_SETTINGS
};

class Kernel : public AppLogic {
    struct {
        unsigned char* pixels = nullptr;
        int width, height, channels;

        GLuint tex;
        struct nk_image image;
    } background;

    std::vector<std::string> assetPaths;

    // available areas
    std::vector<AreaEntry> areas;

    // available animals
    std::vector<AnimalEntry> availableAnimals;

    // available weapons
    std::vector<WeaponEntry> availableWeapons;

    // available perks



    GameState state = GS_CHOOSEPROFILE;
    HuntConfig huntConfig;

    friend class Hunt;
    GameSettings gameSettings;

protected:
    std::vector<Profile> profiles {};
    int selectedProfile = 0;

    // active hunt information. This is the big kahuna after settings everything up
    std::unique_ptr<Hunt> activeHunt = nullptr;

private:
    void LoadProfiles();
    void SaveProfiles();

    void LoadAssetPaths();
    void SaveAssetPaths();

    void LoadGameSettings();
    void SaveGameSettings();

public:
    Kernel();
    ~Kernel();

    void input(SDL_Event *e) override;
    void update(double dt) override;
    void gui(nk_context* ctx) override;
    void render() override;

    void drawMainMenu(nk_context *ctx);
    void drawHuntSetup(nk_context *ctx);
    void drawSettings(nk_context *ctx);
    void drawProfileSelect(nk_context *ctx);

    void LoadAvailableAreas(std::string &assetPrefix, const std::vector<int> &areas);
    void LoadAvailableAnimals(std::string &prefix, const std::vector<int> &animalPrices, const std::vector<OCARN2::Dino>& animals);
    void LoadAvailableWeapons(std::string &prefix, std::vector<int> prices, std::vector<OCARN2::Weapon> weapons);

    void LaunchHunt(AreaEntry area, std::vector<AnimalEntry> animals, std::vector<WeaponEntry> weapons);


    const std::vector<std::string>& GetAssetPaths();
};

#endif //PREDATION_KERNEL_H
