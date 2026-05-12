#ifndef MMU_H
#define MMU_H

#include "../cartridge/cartridge.h"
#include "../constants/enums.h"
#include "../constants/constants.h"
#include <chrono>
#include <cstdint>
#include <array>
#include <functional>

struct MemoryWriteEvent {
  uint16_t address;
  uint8_t value;
  std::chrono::time_point<std::chrono::high_resolution_clock> eventTime;
};

class MMU {
  public:
    MMU(Cartridge& cartridge);

    uint8_t Read(uint16_t address);
    void Write(uint16_t address, uint8_t value);
    void RawWrite(uint16_t address, uint8_t value);
    void ExecuteDMATransfer(uint8_t value);

    bool ReadIORegisterBit(uint16_t reg, uint8_t flag, bool ppuAccess = false) const;
    void WriteIORegisterBit(uint16_t reg, uint8_t flag, bool value);

    Enums::LcdMode ReadLCDMode() const;
    void WriteLCDMode(const Enums::LcdMode lcdMode);

    void RegisterOnAddrWrite(std::function<void (MemoryWriteEvent)> eventFunc);

  private:
    Cartridge& cartridge;
    std::vector<std::function<void (MemoryWriteEvent)>> eventFuncList;
    Enums::LcdMode currentPpuMode = Enums::LcdMode::HBlank;
    uint8_t memory[Constants::MemorySize];
};

#endif