#include "instructions.h"
#include "registers.h"

Instructions::Instructions(Registers* _registers, Memory* _memory)
  : registers(_registers),
    memory(_memory)
{
}

void Instructions::Execute(uint8_t opcode, unsigned char operand) {
  switch (opcode) {
    case 0x00: // NOP
      break;
    case 0x01: // LD BC,nn
      registers->bc = operand;
      break;
    case 0x02: // LD (BC),A
      memory->Write(registers->bc, registers->a);
      break;
    case 0x03: // INC BC
      registers->bc++;
      break;
    case 0x04: // INC B
      registers->b++;
      break;
    case 0x05: // DEC B
      registers->b--;
      break;
    case 0x06: // LD B,n
      registers->b = operand;
      break;
    case 0x07: // RLC A
      {
        unsigned char carry = (registers->a & 0x80) >> 7;
        if(carry) {
          registers->SetFlag(FLAGS_CARRY, true);
        }
        else {
          registers->SetFlag(FLAGS_CARRY, false);
        }

        registers->a <<= 1;
        registers->a += carry;

        registers->SetFlag(FLAGS_NEGATIVE | FLAGS_ZERO | FLAGS_HALFCARRY, false);
      }
      break;
    case 0x08: // LD (nn),SP
      memory->Write(operand, registers->sp);
      break;
    case 0x09: // ADD HL,BC
    {
      uint32_t result = registers->hl + registers->bc;
      registers->SetFlag(FLAGS_HALFCARRY, ((registers->hl & 0x0FFF) + (registers->bc & 0x0FFF)) > 0x0FFF);
      registers->SetFlag(FLAGS_CARRY, result > 0xFFFF);
      registers->SetFlag(FLAGS_NEGATIVE, false);
      registers->hl = result & 0xFFFF;
      break;
    }
    case 0x0A: // LD A,(BC)
      registers->a = memory->Read(registers->bc);
      break;
    case 0x0B: // DEC BC
      registers->bc--;
      break;
    case 0x0C: // INC C
      registers->c++;
      break;
    case 0x0D: // DEC C
      registers->c--;
      break;
    case 0x0E: // LD C,n
      registers->c = operand;
      break;
    case 0x0F: // RRC A
    {
      unsigned char carry = registers->a & 0x01;
      if (carry) {
        registers->SetFlag(FLAGS_CARRY, true);
      }
      else {
        registers->SetFlag(FLAGS_CARRY, false);
      }

      registers->a >>= 1;
      
      if (carry) {
        registers->a |= 0x80;
      }

      registers->SetFlag(FLAGS_NEGATIVE | FLAGS_ZERO | FLAGS_HALFCARRY, false);
    }
      
  }
}