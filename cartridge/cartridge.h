#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <cstdint>
#include <string>

class Cartridge {
  public:
    Cartridge() = default;
    explicit Cartridge(
      const std::string path
    );

    uint8_t Read(
      const uint16_t address
    );
    void SelectRomBank(
      const uint8_t bankNr
    );
    long cartridgeSize;
    bool supported = false;
    std::string cartridgePath;
  private:
    char* cartridgeData;
    uint8_t selectedBankNr = 1;
};

#endif