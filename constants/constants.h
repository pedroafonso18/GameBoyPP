#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cstdint>

namespace Constants
{
  inline const uint32_t MemorySize = 0x10000;    
  inline const uint8_t LcdWidth = 160;
  inline const uint8_t LcdHeight = 144;
  inline const uint8_t TotalSprites = 40;
  inline const uint8_t MaxSpritesY = 10;

  inline const uint16_t AddrCartType = 0x0147;
  inline const uint16_t AddrCartSwitchTriggerStart = 0x2000;
  inline const uint16_t AddrCartSwitchTriggerEnd = 0x3FFF;
  inline const uint16_t AddrSwitchBankStart = 0x4000;
  inline const uint16_t AddrSwitchBankEnd = 0x7FFF;
  inline const uint8_t CartTypeRom = 0x0;
  inline const uint8_t CartTypeMBC1 = 0x1;
};

#endif