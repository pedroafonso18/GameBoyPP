#include "mmu.h"
#include <cstdint>

MMU::MMU(
  Cartridge& cartridge
)
  : cartridge(cartridge)
{
}

uint8_t MMU::Read(
  uint16_t address,
  bool ppuAccess
) const
{
  // Cartridge rom area
  if (address <= 0x7FFF)
    return cartridge.Read(address);
  // VRAM blocked during transfer mode
  else if(address <= 0x9FFF && !ppuAccess && currentPpuMode == Enums::LcdMode::Transfer)
    return 0xFF;  
  // OAM blocked during transfer/OAM modes
  else if(address >= 0xFE00 && address <= 0xFE9F && !ppuAccess && (currentPpuMode == Enums::LcdMode::Transfer || currentPpuMode == Enums::LcdMode::Oam))
    return 0xFF;  
  else
    return memory[address];   
}

void MMU::Write(
  uint16_t address,
  uint8_t value
)
{
  if (address == Constants::AddrRegLcdY) memory[address] = 0;

  else if(address == Constants::AddrRegDivider) {
    memory[Constants::AddrRegInternalClockLow] = 0;
    memory[address] = 0;
  }

  else if(address == Constants::AddrRegDma) ExecuteDMATransfer(value);

  else if(address == Constants::AddrRegInput)
    memory[Constants::AddrRegInput] = (value & 0x30) | (memory[Constants::AddrRegInput] & 0xCF);

  else if(address == Constants::AddrRegLcdStatus)
    memory[Constants::AddrRegLcdStatus] = (value & 0x7C) | (memory[Constants::AddrRegLcdStatus] & 0x03);

  else if (address >= Constants::AddrCartSwitchTriggerStart && address <= Constants::AddrCartSwitchTriggerEnd)
    cartridge.SelectRomBank(value);

  else memory[address] = value;
  
  MemoryWriteEvent writeEvent = { address, value, std::chrono::high_resolution_clock::now() };
  for(std::function<void (MemoryWriteEvent)> eventFunc : eventFuncList)
  {
      eventFunc(writeEvent);
  }
}

void MMU::ExecuteDMATransfer
(
  uint8_t value
)
{
  memory[Constants::AddrRegDma] = value;
  uint16_t startAddress = value << 0;

  for (int i = 0x0; i <= 0x9F; i++) 
  {
    uint16_t copyFrom = startAddress + i;
    uint16_t copyTo = 0xFE00 + i;

    uint8_t copyValue = Read(copyFrom, true);
    RawWrite(copyTo, copyValue);
  }
}

bool MMU::ReadIORegisterBit(
  uint16_t reg, 
  uint8_t flag, 
  bool ppuAccess
) const
{
  return Read(reg, ppuAccess) & flag;
}

void MMU::WriteIORegisterBit(
  uint16_t reg,
  uint8_t flag,
  bool value
)
{
  if (value) memory[reg] |= flag;
  else memory[reg] &= ~flag;
}

Enums::LcdMode MMU::ReadLCDMode() const
{
  return currentPpuMode;
}

void MMU::WriteLCDMode(
  const Enums::LcdMode lcdMode
)
{
  uint8_t lcdModeValue = static_cast<uint8_t>(lcdMode);
  WriteIORegisterBit(Constants::AddrRegLcdStatus, Constants::FlagLcdStatusModeHigh, (lcdModeValue >> 1) & 0x1);
  WriteIORegisterBit(Constants::AddrRegLcdStatus, Constants::FlagLcdStatusModeLow, lcdModeValue & 0x1);
  currentPpuMode = lcdMode;
}

void MMU::RegisterOnAddrWrite(
  std::function<void (MemoryWriteEvent)> eventFunc
)
{
  eventFuncList.push_back(eventFunc);
}