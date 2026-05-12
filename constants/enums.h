#ifndef ENUMS_H
#define ENUMS_H

namespace Enums {
  enum class CpuState {
    STEP,
    PAUSED,
    RUNNING,
    INTERRUPTED,
    ERROR
  };

  enum class LcdMode {
    HBlank,
    VBlank,
    Oam,
    Transfer
  };

  enum class PadButton {
    DOWN,
    UP,
    LEFT,
    RIGHT,
    START,
    SELECT,
    B,
    A
  };

  enum class ColorShade {
    WHITE,
    LIGHT_GRAY,
    DARK_GRAY,
    BLACK,
    TRANSPARENT
  };
};

#endif