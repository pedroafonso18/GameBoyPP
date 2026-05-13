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

  inline const uint16_t AddrTileData0Start = 0x8800;
  inline const uint16_t AddrTileData0End = 0x97FF;
  inline const uint16_t AddrTileData1Start = 0x8000;
  inline const uint16_t AddrTileData1End = 0x8FFF;
  inline const uint16_t AddrBgMap0Start = 0x9800;
  inline const uint16_t AddrBgMap0End = 0x9BFF;
  inline const uint16_t AddrBgMap1Start = 0x9C00;     
  inline const uint16_t AddrBgMap1End = 0x9FFF;
  inline const uint16_t AddrOAMStart = 0xFE00;
  inline const uint16_t AddrOAMEnd = 0xFE9F;
  inline const uint16_t AddrRegInput = 0xFF00;
  inline const uint16_t AddrRegInternalClockLow = 0xFF03;
  inline const uint16_t AddrRegDivider = 0xFF04;
  inline const uint16_t AddrRegTIMA = 0xFF05;
  inline const uint16_t AddrRegTMA = 0xFF06;
  inline const uint16_t AddrRegTAC = 0xFF07;        

  inline const uint16_t AddrRegLcdControl = 0xFF40;
  inline const uint16_t AddrRegLcdStatus = 0xFF41;
  inline const uint16_t AddrRegScrollY = 0xFF42;
  inline const uint16_t AddrRegScrollX = 0xFF43;
  inline const uint16_t AddrRegLcdY = 0xFF44;
  inline const uint16_t AddrRegLcdYCompare = 0xFF45;    
  inline const uint16_t AddrRegDma = 0xFF46;
  inline const uint16_t AddrRegBgPalette = 0xFF47;
  inline const uint16_t AddrRegOBPalette0 = 0xFF48;
  inline const uint16_t AddrRegOBPalette1 = 0xFF49;
  inline const uint16_t AddrRegWindowY = 0xFF4A;
  inline const uint16_t AddrRegWindowX = 0xFF4B;
  inline const uint16_t AddrRegInterruptFlag = 0xFF0F;
  inline const uint16_t AddrRegInterruptEnabled = 0xFFFF;
  inline const uint8_t FlagPadButtonsSet = 32;
  inline const uint8_t FlagPadDirectionSet = 16;    

  inline const uint8_t FlagTimerStart = 4;
  inline const uint8_t FlagTimerClockMode = 3;

  inline const uint8_t FlagInterruptInput = 16;    
  inline const uint8_t FlagInterruptSerial = 8;
  inline const uint8_t FlagInterruptTimer = 4;
  inline const uint8_t FlagInterruptLcd = 2;
  inline const uint8_t FlagInterruptVBlank = 1;

  inline const uint8_t FlagLcdControlLcdOn = 128;
  inline const uint8_t FlagLcdControlWindowMap = 64;
  inline const uint8_t FlagLcdControlWindowOn = 32;
  inline const uint8_t FlagLcdControlBgData = 16;
  inline const uint8_t FlagLcdControlBgMap = 8;
  inline const uint8_t FlagLcdControlObjSize = 4;
  inline const uint8_t FlagLcdControlObjOn = 2;
  inline const uint8_t FlagLcdControlBgOn = 1;

  inline const uint8_t FlagLcdStatusLcdYCInterruptOn = 64;
  inline const uint8_t FlagLcdStatusOamInterruptOn = 32;
  inline const uint8_t FlagLcdStatusVBlankInterruptOn = 16;
  inline const uint8_t FlagLcdStatusHBlankInterruptOn = 8;
  inline const uint8_t FlagLcdStatusCoincidence = 4;
  inline const uint8_t FlagLcdStatusModeHigh = 2;
  inline const uint8_t FlagLcdStatusModeLow = 1; 
};

#endif