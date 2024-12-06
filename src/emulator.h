#pragma once

#include "instruction.h"
#include <cstdint>
class Emulator {
public:
  bool emu_loop(uint8_t *insns);
  bool execute_insn(Instruction &insn);

private:
};
