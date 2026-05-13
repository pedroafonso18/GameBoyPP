#ifndef COLOR_PALETTS_H
#define COLOR_PALETTS_H

#include "../constants/enums.h"

struct ColorPalette {
  Enums::ColorShade colors[4];
};

class ColorPalettes {
  public:
    ColorPalettes();

    ColorPalette GetBWPalette();
    ColorPalette GetOBP0Palette();
    ColorPalette GetOBP1Palette();

  private:
    void onMmuWrite();
};

#endif