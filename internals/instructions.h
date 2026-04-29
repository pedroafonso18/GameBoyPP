#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <cstdint>
#include "memory.h"
#include "registers.h"

class Instructions {
  public:
    Instructions(Registers* registers, Memory* memory);
    void Execute(uint8_t opcode, unsigned char operand);
  private:
    Registers *registers;
    Memory* memory;
};

#endif