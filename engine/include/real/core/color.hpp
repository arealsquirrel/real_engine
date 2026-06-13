#ifndef REALLIB_COLOR_HPP
#define REALLIB_COLOR_HPP

#include <real/core/core.hpp>

namespace real {

/**
 * @brief forces all colors in the color4 to be from within 0-1
 */
struct REALLIB_EXPORT Color4 {
    Color4(float _r, float _g, float _b, float _a=256)
        : r(_r), g(_g), b(_b), a(_a) {}

    Color4(int _r, int _g, int _b, int _a=256)
        : r(_r/255.0f), g(_g/255.0f), b(_b/255.0f), a(_a/256.0f) {}

    float r;
    float g;
    float b;
    float a;
};

}

#endif