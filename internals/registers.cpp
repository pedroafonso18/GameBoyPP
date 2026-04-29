#include "registers.h"
#include <cstdint>

bool Registers::IsFlagSet(uint8_t flag) {
  return f & flag;
}

void Registers::SetFlag(uint8_t flag, bool state) {
  f = state ? (f | flag) : (f & ~flag);
}