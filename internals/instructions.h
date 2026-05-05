#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <cstdint>
#include "memory.h"
#include "cpu.h"
#include "registers.h"

class Instructions {
  public:
    Instructions(Registers* registers, Memory* memory);
    ~Instructions();  
    void Execute(uint8_t opcode, uint16_t operand);
  private:
    Registers *registers;
    Memory* memory;
    Cpu* cpu;
};

#endif