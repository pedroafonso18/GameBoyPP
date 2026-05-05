#include "cpu.h"

bool Cpu::GetStopped() {
    return stopped;
}

void Cpu::SetStopped(bool flag) {
    stopped = flag;
}