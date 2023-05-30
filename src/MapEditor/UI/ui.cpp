//
// Created by kkeiper1103 on 5/25/2023.
//

#include "ui.h"

char buffer[255];
/**
 *
 * @param context
 * @param pValue
 * @param title
 * @param min
 * @param max
 * @param step
 */
void ui_slider_widget(nk_context* context, float* pValue, const char* title, float min, float max, float step) {
    nk_label(context, title, NK_TEXT_ALIGN_LEFT);
    nk_slider_float(context, min, pValue, max, step);
    nk_spacer(context); sprintf(buffer, "%.2f", *pValue);
    nk_label(context, buffer, NK_TEXT_ALIGN_LEFT);
}

/**
 *
 * @param context
 * @param pValue
 */
void ui_seed_widget(nk_context* context, unsigned int* pValue) {
    static EditString seed;
    assert(pValue != nullptr && "Pointer for Seed Buffer Can't be Null!");

    nk_label(context, "Map Seed", NK_TEXT_ALIGN_LEFT);
    nk_edit_string(context, NK_EDIT_FIELD, seed.text, &seed.len, seed.MAX_LEN, nk_filter_default);

    try {
        (*pValue) = std::stoi(seed.text);
    }
    catch(...) {
        (*pValue) = fnv1aHash(seed.text);
    }
    nk_spacer(context);
    nk_label(context, std::to_string(*pValue).c_str(), NK_TEXT_ALIGN_LEFT);
}

/**
 *
 * @param ctx
 * @param text
 * @param onClick
 */
void ui_button_widget(nk_context* ctx, const char* text, const std::function<void()>& onClick) {
    nk_layout_row_dynamic(ctx, 35, 1);
    if(nk_button_label(ctx, text)) onClick();
}

/**
 *
 * @param str
 * @return
 */
unsigned int fnv1aHash(const std::string& str) {
    unsigned int hash = FNV_OFFSET_BASIS;

    for (char c : str) {
        hash ^= static_cast<unsigned int>(c);
        hash *= FNV_PRIME;
    }

    return hash;
}

void ui_file_dialog(nk_context *ctx, const char *label, const std::vector<nfdfilteritem_t>& filters, const std::function<void(const char *)> &onSelect) {
    ui_button_widget(ctx, label, [&]() {

        auto window = SDL_GL_GetCurrentWindow();
        SDL_MinimizeWindow(window);

        nfdchar_t *outPath;
        if( NFD_OKAY == NFD_OpenDialog(&outPath, filters.data(), filters.size(), nullptr) ) {
            onSelect(outPath);

            NFD_FreePath(outPath);
        }

        SDL_MaximizeWindow(window);
    });
}

void ui_folder_dialog(nk_context *ctx, const char *label, const std::function<void(const char *)> &onSelect) {
    ui_button_widget(ctx, label, [&]() {

        auto window = SDL_GL_GetCurrentWindow();
        SDL_MinimizeWindow(window);

        nfdchar_t *outPath;
        if (NFD_OKAY == NFD_PickFolder(&outPath, "")) {
            onSelect(outPath);

            NFD_FreePath(outPath);
        }

        SDL_MaximizeWindow(window);
    });
}
