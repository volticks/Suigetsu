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
  bool execute_insn(const Instruction &ins);

  // Handlers for each instruction
  bool handle_mov(const Instruction &ins);
  bool handle_ext(const Instruction &ins);
  bool handle_add(const Instruction &ins, int sign);
  bool handle_mul(const Instruction &ins);
  bool handle_div(const Instruction &ins);
  bool handle_inc(const Instruction &ins);
  bool handle_cmp(const Instruction &ins);
  bool handle_and(const Instruction &ins);
  bool handle_or(const Instruction &ins);
  bool handle_xor(const Instruction &ins);
  bool handle_not(const Instruction &ins);
  // TODO: Fill in these...

  // Get value either imm/whatever and sign extend (if applicable) or just
  // return it.
  reg_type get_val(ArgKind src, const Instruction &ins);

private:
  Reg regs;
  Decoder decoder;
  MMU mmu;
};
