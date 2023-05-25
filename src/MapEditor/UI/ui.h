//
// Created by kkeiper1103 on 5/25/2023.
//

#ifndef PREDATION_UI_H
#define PREDATION_UI_H

#include <functional>
#include <string>
#include <cassert>

#include <nuklear/nuklear.h>


struct EditString {
    static constexpr int MAX_LEN = 64;
    char text[MAX_LEN] = "0";
    int len {1};
};

constexpr unsigned int FNV_PRIME = 16777619u;
constexpr unsigned int FNV_OFFSET_BASIS = 2166136261u;

unsigned int fnv1aHash(const std::string& str);
void ui_slider_widget(nk_context* context, float* pValue, const char* title, float min = 0, float max = 1, float step = 0.01);
void ui_seed_widget(nk_context* context, unsigned int* pValue);
void ui_button_widget(nk_context* ctx, const char* text, const std::function<void()>& onClick);


#endif //PREDATION_UI_H
