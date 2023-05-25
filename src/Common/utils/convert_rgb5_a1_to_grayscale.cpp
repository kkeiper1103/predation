//
// Created by kkeiper1103 on 12/22/22.
//

#include <cmath>
#include "convert_rgb5_a1_to_grayscale.h"

std::vector<unsigned short> to_grayscale(unsigned short *texture, int width, int height) {
    std::vector<unsigned short> grayscale(width * height, 0);

    for(int z = 0; z < 256; z++) {
        for(int x = 0; x < 256; x++) {
            unsigned short pixel = texture[z * 256 + x];

            // 31 is the top of the range, due to 5 bits of fidelity 2^5 = 32
            unsigned short b = (pixel >> 0) & 31,
                    g = (pixel >> 5) & 31,
                    r = (pixel >> 10) & 31;

            // CIE colors for how sensitive human rods are to certain colors
            // green is highest sensitive, then red, then barely blue
            auto factor = (float) (.299 * r + .587 * g + .114 * b) / 31;

            // multiply by 31 to get the final value for the pixel
            b = g = r = floor(factor * 31);

            grayscale[z * 256 + x] = b + (g << 5) + (r << 10) + (0<<15);
        }
    }

    return grayscale;
}
