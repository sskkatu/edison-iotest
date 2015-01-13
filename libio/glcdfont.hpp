#pragma once

#include <cstdint>

class GLcdFont {
private:
    static const uint8_t glcdfont[];

public:     
    static const uint8_t* get5x7FontBitmap(uint8_t c) {
        return GLcdFont::glcdfont + c * 5;
    }
};

