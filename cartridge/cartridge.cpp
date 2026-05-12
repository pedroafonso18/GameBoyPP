#include "cartridge.h"

#include <fstream>
#include "../constants/constants.h"

Cartridge::Cartridge(
  const std::string path
) : 
  cartridgePath(path)
{
  std::fstream cartridgeFile;
  cartridgeFile.open(cartridgePath, std::ifstream::binary);

  cartridgeFile.seekg(0, cartridgeFile.end);
  cartridgeSize = (unsigned long) cartridgeFile.tellg();
  cartridgeFile.seekg(0, cartridgeFile.beg);

  cartridgeData = new char[cartridgeSize];
  cartridgeFile.read(cartridgeData, cartridgeSize);
  cartridgeFile.close();

  supported = cartridgeData[Constants::AddrCartType] >= Constants::CartTypeRom
    && cartridgeData[Constants::AddrCartType] <= Constants::CartTypeMBC1;
}

uint8_t Cartridge::Read(uint16_t address) {
  uint8_t returnval = address >= Constants::AddrSwitchBankStart && address <= Constants::AddrSwitchBankEnd ?
    (uint8_t)cartridgeData[address + ((selectedBankNr - 1) * 0x4000)] :
    (uint8_t)cartridgeData[address];
  return returnval; 
}

void Cartridge::SelectRomBank(
  const uint8_t bankNr
) 
{
  selectedBankNr = bankNr;
}