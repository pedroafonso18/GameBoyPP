#include "instructions.h"
#include "registers.h"

Instructions::Instructions(Registers* _registers, Memory* _memory)
  : registers(_registers),
    memory(_memory),
    cpu(new Cpu())
{
}

Instructions::~Instructions()
{
  delete cpu;
}

namespace {

uint8_t ReadRegister8(Registers* registers, Memory* memory, uint8_t index)
{
  switch (index & 0x07) {
    case 0: return registers->b;
    case 1: return registers->c;
    case 2: return registers->d;
    case 3: return registers->e;
    case 4: return registers->h;
    case 5: return registers->l;
    case 6: return memory->Read(registers->hl);
    default: return registers->a;
  }
}

void WriteRegister8(Registers* registers, Memory* memory, uint8_t index, uint8_t value)
{
  switch (index & 0x07) {
    case 0: registers->b = value; break;
    case 1: registers->c = value; break;
    case 2: registers->d = value; break;
    case 3: registers->e = value; break;
    case 4: registers->h = value; break;
    case 5: registers->l = value; break;
    case 6: memory->Write(registers->hl, value); break;
    default: registers->a = value; break;
  }
}

uint16_t ReadRegister16(Registers* registers, uint8_t index)
{
  switch (index & 0x03) {
    case 0: return registers->bc;
    case 1: return registers->de;
    case 2: return registers->hl;
    default: return registers->fa;
  }
}

void WriteRegister16(Registers* registers, uint8_t index, uint16_t value)
{
  switch (index & 0x03) {
    case 0: registers->bc = value; break;
    case 1: registers->de = value; break;
    case 2: registers->hl = value; break;
    default: registers->fa = value & 0xFFF0; break;
  }
}

void Push16(Registers* registers, Memory* memory, uint16_t value)
{
  registers->sp--;
  memory->Write(registers->sp, static_cast<uint8_t>((value >> 8) & 0xFF));
  registers->sp--;
  memory->Write(registers->sp, static_cast<uint8_t>(value & 0xFF));
}

uint16_t Pop16(Registers* registers, Memory* memory)
{
  uint8_t low = memory->Read(registers->sp);
  registers->sp++;
  uint8_t high = memory->Read(registers->sp);
  registers->sp++;
  return static_cast<uint16_t>(low | (high << 8));
}

uint8_t Inc8(Registers* registers, uint8_t value)
{
  uint8_t result = static_cast<uint8_t>(value + 1);
  registers->SetFlag(FLAGS_ZERO, result == 0);
  registers->SetFlag(FLAGS_NEGATIVE, false);
  registers->SetFlag(FLAGS_HALFCARRY, (value & 0x0F) == 0x0F);
  return result;
}

uint8_t Dec8(Registers* registers, uint8_t value)
{
  uint8_t result = static_cast<uint8_t>(value - 1);
  registers->SetFlag(FLAGS_ZERO, result == 0);
  registers->SetFlag(FLAGS_NEGATIVE, true);
  registers->SetFlag(FLAGS_HALFCARRY, (value & 0x0F) == 0x00);
  return result;
}

void AddA(Registers* registers, uint8_t value, bool withCarry)
{
  uint8_t carryIn = withCarry && registers->IsFlagSet(FLAGS_CARRY) ? 1 : 0;
  uint16_t sum = static_cast<uint16_t>(registers->a) + value + carryIn;
  registers->SetFlag(FLAGS_ZERO, static_cast<uint8_t>(sum) == 0);
  registers->SetFlag(FLAGS_NEGATIVE, false);
  registers->SetFlag(FLAGS_HALFCARRY, ((registers->a & 0x0F) + (value & 0x0F) + carryIn) > 0x0F);
  registers->SetFlag(FLAGS_CARRY, sum > 0xFF);
  registers->a = static_cast<uint8_t>(sum & 0xFF);
}

void SubA(Registers* registers, uint8_t value, bool withCarry)
{
  uint8_t carryIn = withCarry && registers->IsFlagSet(FLAGS_CARRY) ? 1 : 0;
  uint16_t minuend = registers->a;
  uint16_t subtrahend = static_cast<uint16_t>(value) + carryIn;
  uint16_t result = static_cast<uint16_t>(minuend - subtrahend);
  registers->SetFlag(FLAGS_ZERO, static_cast<uint8_t>(result) == 0);
  registers->SetFlag(FLAGS_NEGATIVE, true);
  registers->SetFlag(FLAGS_HALFCARRY, (registers->a & 0x0F) < ((value & 0x0F) + carryIn));
  registers->SetFlag(FLAGS_CARRY, minuend < subtrahend);
  registers->a = static_cast<uint8_t>(result & 0xFF);
}

void AndA(Registers* registers, uint8_t value)
{
  registers->a &= value;
  registers->SetFlag(FLAGS_ZERO, registers->a == 0);
  registers->SetFlag(FLAGS_NEGATIVE, false);
  registers->SetFlag(FLAGS_HALFCARRY, true);
  registers->SetFlag(FLAGS_CARRY, false);
}

void XorA(Registers* registers, uint8_t value)
{
  registers->a ^= value;
  registers->SetFlag(FLAGS_ZERO, registers->a == 0);
  registers->SetFlag(FLAGS_NEGATIVE, false);
  registers->SetFlag(FLAGS_HALFCARRY, false);
  registers->SetFlag(FLAGS_CARRY, false);
}

void OrA(Registers* registers, uint8_t value)
{
  registers->a |= value;
  registers->SetFlag(FLAGS_ZERO, registers->a == 0);
  registers->SetFlag(FLAGS_NEGATIVE, false);
  registers->SetFlag(FLAGS_HALFCARRY, false);
  registers->SetFlag(FLAGS_CARRY, false);
}

void CpA(Registers* registers, uint8_t value)
{
  uint8_t current = registers->a;
  registers->SetFlag(FLAGS_ZERO, current == value);
  registers->SetFlag(FLAGS_NEGATIVE, true);
  registers->SetFlag(FLAGS_HALFCARRY, (current & 0x0F) < (value & 0x0F));
  registers->SetFlag(FLAGS_CARRY, current < value);
}

uint8_t RotateLeft(Registers* registers, uint8_t value, bool throughCarry, bool setZero)
{
  uint8_t carryIn = throughCarry && registers->IsFlagSet(FLAGS_CARRY) ? 1 : 0;
  uint8_t carryOut = static_cast<uint8_t>((value & 0x80) >> 7);
  uint8_t result = static_cast<uint8_t>((value << 1) | carryIn);
  registers->SetFlag(FLAGS_ZERO, setZero && result == 0);
  registers->SetFlag(FLAGS_NEGATIVE, false);
  registers->SetFlag(FLAGS_HALFCARRY, false);
  registers->SetFlag(FLAGS_CARRY, carryOut != 0);
  return result;
}

uint8_t RotateRight(Registers* registers, uint8_t value, bool throughCarry, bool setZero)
{
  uint8_t carryIn = throughCarry && registers->IsFlagSet(FLAGS_CARRY) ? 0x80 : 0;
  uint8_t carryOut = static_cast<uint8_t>(value & 0x01);
  uint8_t result = static_cast<uint8_t>((value >> 1) | carryIn);
  registers->SetFlag(FLAGS_ZERO, setZero && result == 0);
  registers->SetFlag(FLAGS_NEGATIVE, false);
  registers->SetFlag(FLAGS_HALFCARRY, false);
  registers->SetFlag(FLAGS_CARRY, carryOut != 0);
  return result;
}

uint8_t ShiftLeft(Registers* registers, uint8_t value, bool setZero)
{
  uint8_t result = static_cast<uint8_t>(value << 1);
  registers->SetFlag(FLAGS_ZERO, setZero && result == 0);
  registers->SetFlag(FLAGS_NEGATIVE, false);
  registers->SetFlag(FLAGS_HALFCARRY, false);
  registers->SetFlag(FLAGS_CARRY, (value & 0x80) != 0);
  return result;
}

uint8_t ShiftRightArithmetic(Registers* registers, uint8_t value, bool setZero)
{
  uint8_t result = static_cast<uint8_t>((value >> 1) | (value & 0x80));
  registers->SetFlag(FLAGS_ZERO, setZero && result == 0);
  registers->SetFlag(FLAGS_NEGATIVE, false);
  registers->SetFlag(FLAGS_HALFCARRY, false);
  registers->SetFlag(FLAGS_CARRY, (value & 0x01) != 0);
  return result;
}

uint8_t ShiftRightLogical(Registers* registers, uint8_t value, bool setZero)
{
  uint8_t result = static_cast<uint8_t>(value >> 1);
  registers->SetFlag(FLAGS_ZERO, setZero && result == 0);
  registers->SetFlag(FLAGS_NEGATIVE, false);
  registers->SetFlag(FLAGS_HALFCARRY, false);
  registers->SetFlag(FLAGS_CARRY, (value & 0x01) != 0);
  return result;
}

uint8_t SwapNibbles(Registers* registers, uint8_t value, bool setZero)
{
  uint8_t result = static_cast<uint8_t>((value << 4) | (value >> 4));
  registers->SetFlag(FLAGS_ZERO, setZero && result == 0);
  registers->SetFlag(FLAGS_NEGATIVE, false);
  registers->SetFlag(FLAGS_HALFCARRY, false);
  registers->SetFlag(FLAGS_CARRY, false);
  return result;
}

bool CheckCondition(Registers* registers, uint8_t opcode)
{
  switch (opcode) {
    case 0x20:
    case 0xC2:
    case 0xC4:
    case 0xC0:
      return !registers->IsFlagSet(FLAGS_ZERO);
    case 0x28:
    case 0xCA:
    case 0xCC:
    case 0xC8:
      return registers->IsFlagSet(FLAGS_ZERO);
    case 0x30:
    case 0xD2:
    case 0xD4:
    case 0xD0:
      return !registers->IsFlagSet(FLAGS_CARRY);
    case 0x38:
    case 0xDA:
    case 0xDC:
    case 0xD8:
      return registers->IsFlagSet(FLAGS_CARRY);
    default:
      return false;
  }
}

void Daa(Registers* registers)
{
  uint8_t a = registers->a;
  uint8_t adjust = 0;
  bool carry = registers->IsFlagSet(FLAGS_CARRY);

  if (!registers->IsFlagSet(FLAGS_NEGATIVE)) {
    if (carry || a > 0x99) {
      adjust |= 0x60;
      carry = true;
    }
    if (registers->IsFlagSet(FLAGS_HALFCARRY) || (a & 0x0F) > 0x09) {
      adjust |= 0x06;
    }
    a = static_cast<uint8_t>(a + adjust);
  } else {
    if (carry) {
      adjust |= 0x60;
    }
    if (registers->IsFlagSet(FLAGS_HALFCARRY)) {
      adjust |= 0x06;
    }
    a = static_cast<uint8_t>(a - adjust);
  }

  registers->a = a;
  registers->SetFlag(FLAGS_ZERO, a == 0);
  registers->SetFlag(FLAGS_HALFCARRY, false);
  registers->SetFlag(FLAGS_CARRY, carry);
}

void ExecuteCb(Registers* registers, Memory* memory, uint8_t opcode)
{
  uint8_t target = opcode & 0x07;
  uint8_t bit = static_cast<uint8_t>((opcode >> 3) & 0x07);
  uint8_t value = ReadRegister8(registers, memory, target);

  if (opcode < 0x40) {
    switch (opcode >> 3) {
      case 0: value = RotateLeft(registers, value, false, true); break;
      case 1: value = RotateRight(registers, value, false, true); break;
      case 2: value = RotateLeft(registers, value, true, true); break;
      case 3: value = RotateRight(registers, value, true, true); break;
      case 4: value = ShiftLeft(registers, value, true); break;
      case 5: value = ShiftRightArithmetic(registers, value, true); break;
      case 6: value = SwapNibbles(registers, value, true); break;
      default: value = ShiftRightLogical(registers, value, true); break;
    }
    WriteRegister8(registers, memory, target, value);
    return;
  }

  if (opcode < 0x80) {
    registers->SetFlag(FLAGS_ZERO, (value & (1 << bit)) == 0);
    registers->SetFlag(FLAGS_NEGATIVE, false);
    registers->SetFlag(FLAGS_HALFCARRY, true);
    return;
  }

  if (opcode < 0xC0) {
    value = static_cast<uint8_t>(value & ~(1 << bit));
    WriteRegister8(registers, memory, target, value);
    return;
  }

  value = static_cast<uint8_t>(value | (1 << bit));
  WriteRegister8(registers, memory, target, value);
}

}

void Instructions::Execute(uint8_t opcode, uint16_t operand) {
  uint8_t immediate8 = static_cast<uint8_t>(operand & 0xFF);
  uint16_t immediate16 = operand;

  if (opcode == 0xCB) {
    ExecuteCb(registers, memory, immediate8);
    return;
  }

  if ((opcode & 0xC0) == 0x40) {
    if (opcode == 0x76) {
      cpu->SetStopped(true);
      return;
    }

    uint8_t destination = static_cast<uint8_t>((opcode >> 3) & 0x07);
    uint8_t source = static_cast<uint8_t>(opcode & 0x07);
    WriteRegister8(registers, memory, destination, ReadRegister8(registers, memory, source));
    return;
  }

  if ((opcode & 0xC0) == 0x80) {
    uint8_t source = static_cast<uint8_t>(opcode & 0x07);
    uint8_t value = ReadRegister8(registers, memory, source);

    switch ((opcode >> 3) & 0x07) {
      case 0: AddA(registers, value, false); break;
      case 1: AddA(registers, value, true); break;
      case 2: SubA(registers, value, false); break;
      case 3: SubA(registers, value, true); break;
      case 4: AndA(registers, value); break;
      case 5: XorA(registers, value); break;
      case 6: OrA(registers, value); break;
      default: CpA(registers, value); break;
    }
    return;
  }

  if ((opcode & 0xC7) == 0x04) {
    uint8_t target = static_cast<uint8_t>((opcode >> 3) & 0x07);
    uint8_t value = ReadRegister8(registers, memory, target);
    WriteRegister8(registers, memory, target, Inc8(registers, value));
    return;
  }

  if ((opcode & 0xC7) == 0x05) {
    uint8_t target = static_cast<uint8_t>((opcode >> 3) & 0x07);
    uint8_t value = ReadRegister8(registers, memory, target);
    WriteRegister8(registers, memory, target, Dec8(registers, value));
    return;
  }

  switch (opcode) {
    case 0x00:
      break;

    case 0x01:
      registers->bc = immediate16;
      break;

    case 0x02:
      memory->Write(registers->bc, registers->a);
      break;

    case 0x03:
      registers->bc++;
      break;

    case 0x06:
      registers->b = immediate8;
      break;

    case 0x07:
      registers->a = RotateLeft(registers, registers->a, false, false);
      break;

    case 0x08:
      memory->Write(immediate16, static_cast<uint8_t>(registers->sp & 0xFF));
      memory->Write(static_cast<uint16_t>(immediate16 + 1), static_cast<uint8_t>((registers->sp >> 8) & 0xFF));
      break;

    case 0x09:
    {
      uint32_t result = static_cast<uint32_t>(registers->hl) + registers->bc;
      registers->SetFlag(FLAGS_HALFCARRY, ((registers->hl & 0x0FFF) + (registers->bc & 0x0FFF)) > 0x0FFF);
      registers->SetFlag(FLAGS_CARRY, result > 0xFFFF);
      registers->SetFlag(FLAGS_NEGATIVE, false);
      registers->hl = static_cast<uint16_t>(result & 0xFFFF);
      break;
    }

    case 0x0A:
      registers->a = memory->Read(registers->bc);
      break;

    case 0x0B:
      registers->bc--;
      break;

    case 0x0E:
      registers->c = immediate8;
      break;

    case 0x0F:
      registers->a = RotateRight(registers, registers->a, false, false);
      break;

    case 0x10:
      cpu->SetStopped(true);
      break;

    case 0x11:
      registers->de = immediate16;
      break;

    case 0x12:
      memory->Write(registers->de, registers->a);
      break;

    case 0x13:
      registers->de++;
      break;

    case 0x16:
      registers->d = immediate8;
      break;

    case 0x17:
    {
      uint8_t carryIn = registers->IsFlagSet(FLAGS_CARRY) ? 1 : 0;
      uint8_t carryOut = static_cast<uint8_t>((registers->a & 0x80) >> 7);
      registers->a = static_cast<uint8_t>((registers->a << 1) | carryIn);
      registers->SetFlag(FLAGS_ZERO, false);
      registers->SetFlag(FLAGS_NEGATIVE, false);
      registers->SetFlag(FLAGS_HALFCARRY, false);
      registers->SetFlag(FLAGS_CARRY, carryOut != 0);
      break;
    }

    case 0x18:
      registers->pc = static_cast<uint16_t>(registers->pc + static_cast<int8_t>(immediate8));
      break;

    case 0x19:
    {
      uint32_t result = static_cast<uint32_t>(registers->hl) + registers->de;
      registers->SetFlag(FLAGS_HALFCARRY, ((registers->hl & 0x0FFF) + (registers->de & 0x0FFF)) > 0x0FFF);
      registers->SetFlag(FLAGS_CARRY, result > 0xFFFF);
      registers->SetFlag(FLAGS_NEGATIVE, false);
      registers->hl = static_cast<uint16_t>(result & 0xFFFF);
      break;
    }

    case 0x1A:
      registers->a = memory->Read(registers->de);
      break;

    case 0x1B:
      registers->de--;
      break;

    case 0x1E:
      registers->e = immediate8;
      break;

    case 0x1F:
    {
      uint8_t carryIn = registers->IsFlagSet(FLAGS_CARRY) ? 0x80 : 0;
      uint8_t carryOut = static_cast<uint8_t>(registers->a & 0x01);
      registers->a = static_cast<uint8_t>((registers->a >> 1) | carryIn);
      registers->SetFlag(FLAGS_ZERO, false);
      registers->SetFlag(FLAGS_NEGATIVE, false);
      registers->SetFlag(FLAGS_HALFCARRY, false);
      registers->SetFlag(FLAGS_CARRY, carryOut != 0);
      break;
    }

    case 0x20:
      if (!registers->IsFlagSet(FLAGS_ZERO)) {
        registers->pc = static_cast<uint16_t>(registers->pc + static_cast<int8_t>(immediate8));
      }
      break;

    case 0x21:
      registers->hl = immediate16;
      break;

    case 0x22:
      memory->Write(registers->hl, registers->a);
      registers->hl++;
      break;

    case 0x23:
      registers->hl++;
      break;

    case 0x26:
      registers->h = immediate8;
      break;

    case 0x27:
      Daa(registers);
      break;

    case 0x28:
      if (registers->IsFlagSet(FLAGS_ZERO)) {
        registers->pc = static_cast<uint16_t>(registers->pc + static_cast<int8_t>(immediate8));
      }
      break;

    case 0x29:
    {
      uint32_t result = static_cast<uint32_t>(registers->hl) + registers->hl;
      registers->SetFlag(FLAGS_HALFCARRY, ((registers->hl & 0x0FFF) + (registers->hl & 0x0FFF)) > 0x0FFF);
      registers->SetFlag(FLAGS_CARRY, result > 0xFFFF);
      registers->SetFlag(FLAGS_NEGATIVE, false);
      registers->hl = static_cast<uint16_t>(result & 0xFFFF);
      break;
    }

    case 0x2A:
      registers->a = memory->Read(registers->hl);
      registers->hl++;
      break;

    case 0x2B:
      registers->hl--;
      break;

    case 0x2E:
      registers->l = immediate8;
      break;

    case 0x2F:
      registers->a = static_cast<uint8_t>(~registers->a);
      registers->SetFlag(FLAGS_NEGATIVE, true);
      registers->SetFlag(FLAGS_HALFCARRY, true);
      break;

    case 0x30:
      if (!registers->IsFlagSet(FLAGS_CARRY)) {
        registers->pc = static_cast<uint16_t>(registers->pc + static_cast<int8_t>(immediate8));
      }
      break;

    case 0x31:
      registers->sp = immediate16;
      break;

    case 0x32:
      memory->Write(registers->hl, registers->a);
      registers->hl--;
      break;

    case 0x33:
      registers->sp++;
      break;

    case 0x34:
    {
      uint8_t value = memory->Read(registers->hl);
      value = Inc8(registers, value);
      memory->Write(registers->hl, value);
      break;
    }

    case 0x35:
    {
      uint8_t value = memory->Read(registers->hl);
      value = Dec8(registers, value);
      memory->Write(registers->hl, value);
      break;
    }

    case 0x36:
      memory->Write(registers->hl, immediate8);
      break;

    case 0x37:
      registers->SetFlag(FLAGS_CARRY, true);
      registers->SetFlag(FLAGS_NEGATIVE, false);
      registers->SetFlag(FLAGS_HALFCARRY, false);
      break;

    case 0x38:
      if (registers->IsFlagSet(FLAGS_CARRY)) {
        registers->pc = static_cast<uint16_t>(registers->pc + static_cast<int8_t>(immediate8));
      }
      break;

    case 0x39:
    {
      uint32_t result = static_cast<uint32_t>(registers->hl) + registers->sp;
      registers->SetFlag(FLAGS_HALFCARRY, ((registers->hl & 0x0FFF) + (registers->sp & 0x0FFF)) > 0x0FFF);
      registers->SetFlag(FLAGS_CARRY, result > 0xFFFF);
      registers->SetFlag(FLAGS_NEGATIVE, false);
      registers->hl = static_cast<uint16_t>(result & 0xFFFF);
      break;
    }

    case 0x3A:
      registers->a = memory->Read(registers->hl);
      registers->hl--;
      break;

    case 0x3B:
      registers->sp--;
      break;

    case 0x3C:
      registers->a = Inc8(registers, registers->a);
      break;

    case 0x3D:
      registers->a = Dec8(registers, registers->a);
      break;

    case 0x3E:
      registers->a = immediate8;
      break;

    case 0x3F:
      registers->SetFlag(FLAGS_CARRY, !registers->IsFlagSet(FLAGS_CARRY));
      registers->SetFlag(FLAGS_NEGATIVE, false);
      registers->SetFlag(FLAGS_HALFCARRY, false);
      break;

    case 0x76:
      cpu->SetStopped(true);
      break;

    case 0x86:
      AddA(registers, memory->Read(registers->hl), false);
      break;

    case 0x8E:
      AddA(registers, memory->Read(registers->hl), true);
      break;

    case 0x96:
      SubA(registers, memory->Read(registers->hl), false);
      break;

    case 0x9E:
      SubA(registers, memory->Read(registers->hl), true);
      break;

    case 0xA6:
      AndA(registers, memory->Read(registers->hl));
      break;

    case 0xAE:
      XorA(registers, memory->Read(registers->hl));
      break;

    case 0xB6:
      OrA(registers, memory->Read(registers->hl));
      break;

    case 0xBE:
      CpA(registers, memory->Read(registers->hl));
      break;

    case 0xC1:
      registers->bc = Pop16(registers, memory);
      break;

    case 0xC3:
      registers->pc = immediate16;
      break;

    case 0xC5:
      Push16(registers, memory, registers->bc);
      break;

    case 0xC6:
      AddA(registers, immediate8, false);
      break;

    case 0xC7:
      Push16(registers, memory, registers->pc);
      registers->pc = 0x00;
      break;

    case 0xC8:
      if (registers->IsFlagSet(FLAGS_ZERO)) {
        registers->pc = Pop16(registers, memory);
      }
      break;

    case 0xC9:
      registers->pc = Pop16(registers, memory);
      break;

    case 0xCA:
      if (registers->IsFlagSet(FLAGS_ZERO)) {
        registers->pc = immediate16;
      }
      break;

    case 0xCB:
      ExecuteCb(registers, memory, immediate8);
      break;

    case 0xCC:
      if (registers->IsFlagSet(FLAGS_ZERO)) {
        Push16(registers, memory, registers->pc);
        registers->pc = immediate16;
      }
      break;

    case 0xCD:
      Push16(registers, memory, registers->pc);
      registers->pc = immediate16;
      break;

    case 0xCE:
      AddA(registers, immediate8, true);
      break;

    case 0xCF:
      Push16(registers, memory, registers->pc);
      registers->pc = 0x08;
      break;

    case 0xD0:
      if (!registers->IsFlagSet(FLAGS_CARRY)) {
        registers->pc = Pop16(registers, memory);
      }
      break;

    case 0xD1:
      registers->de = Pop16(registers, memory);
      break;

    case 0xD2:
      if (!registers->IsFlagSet(FLAGS_CARRY)) {
        registers->pc = immediate16;
      }
      break;

    case 0xD4:
      if (!registers->IsFlagSet(FLAGS_CARRY)) {
        Push16(registers, memory, registers->pc);
        registers->pc = immediate16;
      }
      break;

    case 0xD5:
      Push16(registers, memory, registers->de);
      break;

    case 0xD6:
      SubA(registers, immediate8, false);
      break;

    case 0xD7:
      Push16(registers, memory, registers->pc);
      registers->pc = 0x10;
      break;

    case 0xD8:
      if (registers->IsFlagSet(FLAGS_CARRY)) {
        registers->pc = Pop16(registers, memory);
      }
      break;

    case 0xD9:
      registers->pc = Pop16(registers, memory);
      break;

    case 0xDA:
      if (registers->IsFlagSet(FLAGS_CARRY)) {
        registers->pc = immediate16;
      }
      break;

    case 0xDE:
      SubA(registers, immediate8, true);
      break;

    case 0xDF:
      Push16(registers, memory, registers->pc);
      registers->pc = 0x18;
      break;

    case 0xE0:
      memory->Write(static_cast<uint16_t>(0xFF00 + immediate8), registers->a);
      break;

    case 0xE1:
      registers->hl = Pop16(registers, memory);
      break;

    case 0xE2:
      memory->Write(static_cast<uint16_t>(0xFF00 + registers->c), registers->a);
      break;

    case 0xE5:
      Push16(registers, memory, registers->hl);
      break;

    case 0xE6:
      AndA(registers, immediate8);
      break;

    case 0xE7:
      Push16(registers, memory, registers->pc);
      registers->pc = 0x20;
      break;

    case 0xE8:
    {
      int8_t offset = static_cast<int8_t>(immediate8);
      uint16_t sp = registers->sp;
      uint16_t result = static_cast<uint16_t>(sp + offset);
      registers->SetFlag(FLAGS_ZERO, false);
      registers->SetFlag(FLAGS_NEGATIVE, false);
      registers->SetFlag(FLAGS_HALFCARRY, ((sp & 0x0F) + (immediate8 & 0x0F)) > 0x0F);
      registers->SetFlag(FLAGS_CARRY, ((sp & 0xFF) + immediate8) > 0xFF);
      registers->sp = result;
      break;
    }

    case 0xE9:
      registers->pc = registers->hl;
      break;

    case 0xEA:
      memory->Write(immediate16, registers->a);
      break;

    case 0xEE:
      XorA(registers, immediate8);
      break;

    case 0xEF:
      Push16(registers, memory, registers->pc);
      registers->pc = 0x28;
      break;

    case 0xF0:
      registers->a = memory->Read(static_cast<uint16_t>(0xFF00 + immediate8));
      break;

    case 0xF1:
      registers->fa = Pop16(registers, memory) & 0xFFF0;
      break;

    case 0xF2:
      registers->a = memory->Read(static_cast<uint16_t>(0xFF00 + registers->c));
      break;

    case 0xF3:
      break;

    case 0xF5:
      Push16(registers, memory, static_cast<uint16_t>(registers->fa & 0xFFF0));
      break;

    case 0xF6:
      OrA(registers, immediate8);
      break;

    case 0xF7:
      Push16(registers, memory, registers->pc);
      registers->pc = 0x30;
      break;

    case 0xF8:
    {
      int8_t offset = static_cast<int8_t>(immediate8);
      uint16_t sp = registers->sp;
      uint16_t result = static_cast<uint16_t>(sp + offset);
      registers->SetFlag(FLAGS_ZERO, false);
      registers->SetFlag(FLAGS_NEGATIVE, false);
      registers->SetFlag(FLAGS_HALFCARRY, ((sp & 0x0F) + (immediate8 & 0x0F)) > 0x0F);
      registers->SetFlag(FLAGS_CARRY, ((sp & 0xFF) + immediate8) > 0xFF);
      registers->hl = result;
      break;
    }

    case 0xF9:
      registers->sp = registers->hl;
      break;

    case 0xFA:
      registers->a = memory->Read(immediate16);
      break;

    case 0xFB:
      break;

    case 0xFE:
      CpA(registers, immediate8);
      break;

    case 0xFF:
      Push16(registers, memory, registers->pc);
      registers->pc = 0x38;
      break;

    default:
      if (CheckCondition(registers, opcode)) {
        if (opcode == 0xC0 || opcode == 0xC8 || opcode == 0xD0 || opcode == 0xD8) {
          registers->pc = Pop16(registers, memory);
        }
      }
      break;
  }
}