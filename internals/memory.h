#ifndef MEMORY_H
#define MEMORY_H

#include <cstdint>
#include <array>

class Memory {
  public:
    
    uint8_t Read(uint16_t address) {
      return data[address];
    }

    void Write(uint16_t address, uint8_t value) {
      data[address] = value;
    }

  private:
    std::array<uint8_t, 0x10000> data{};
};

#endif