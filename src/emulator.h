#pragma once

#include "decoder.h"
#include "instruction.h"
#include "mmu.h"
#include "registers.h"
#include <cstdint>
#include <vector>

typedef std::vector<uint8_t> Instructions;

class Emulator {
public:
  // True: On success
  bool emu_loop(const Instructions &insns);
  bool execute_insn(const Instruction &insn);

private:
  Reg registers;
  Decoder decoder;
  MMU mmu;
};
