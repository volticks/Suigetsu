#pragma once

#include "decoder.h"
#include "instruction.h"
#include "mmu.h"
#include "registers.h"
#include <cstdint>
#include <vector>

// Whether an instruction reads from or writes to memory
enum Op { R, W };
enum MovmBits {
  other = 0x8,
  a3 = 0x10,
  a2 = 0x20,
  d3 = 0x40,
  d2 = 0x80,
};

typedef std::vector<uint8_t> Instructions;

constexpr bool is_reg(ArgKind kind);

constexpr bool is_imm(ArgKind kind);

constexpr bool is_dn(ArgKind kind);

constexpr bool is_abs(ArgKind kind);
constexpr bool is_mem_reg(ArgKind kind);

constexpr bool is_disp(ArgKind kind);
// TODO: May have to expand this to consider memory regs aswell.
constexpr bool is_dn(ArgKind kind) {
  return kind >= ArgKind::D0 && kind <= ArgKind::D3;
}

constexpr bool is_an(ArgKind kind) {
  return kind >= ArgKind::A0 && kind <= ArgKind::A3;
}

constexpr bool is_abs(ArgKind kind) {
  return kind >= ArgKind::abs16 && kind <= ArgKind::abs32;
}
constexpr bool is_mem_reg(ArgKind kind) {
  return kind >= ArgKind::MA0 && kind <= ArgKind::MA3;
}
constexpr bool is_disp(ArgKind kind) {
  return kind >= ArgKind::d8 && kind <= ArgKind::d32;
}

// Sign extend a number of size nbits
constexpr reg_type s_ext(reg_type i, uint32_t nbits);

class Emulator {
public:
  // True: On success
  bool emu_loop(const Instructions &insns, virt_addr start);
  bool execute_insn(const Instruction &ins);

  // Handlers for each instruction
  template <typename T> bool handle_mov(const Instruction &ins);
  bool handle_movm(const Instruction &ins);
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
  bool handle_btst(const Instruction &ins);
  bool handle_bset(const Instruction &ins);
  bool handle_bclr(const Instruction &ins);
  bool handle_asr(const Instruction &ins);
  bool handle_lsr(const Instruction &ins);
  // TODO: Also handle asl2 pls
  bool handle_asl(const Instruction &ins);
  bool handle_ror(const Instruction &ins);
  bool handle_rol(const Instruction &ins);
  bool handle_bcc(const Instruction &ins);
  bool handle_lcc(const Instruction &ins);
  bool handle_setlb(const Instruction &ins);
  bool handle_jmp(const Instruction &ins);

  bool handle_calls(const Instruction &ins);
  bool handle_ret(const Instruction &ins);
  bool handle_rti(const Instruction &ins);
  bool handle_call(const Instruction &ins);
  // ...
  bool handle_trap(const Instruction &ins);
  bool handle_nop(const Instruction &ins);

  // Like get_val but for memory.
  reg_type get_val_mem(const Instruction &ins, Op *operation);
  // Get the address from 2 diff registers - IE, base and displacement.
  reg_type get_addr_from_regs(const Instruction &ins, bool arg);
  // Get value either imm/whatever and sign extend (if applicable) or just
  // return it.
  reg_type get_val(ArgKind src, const Instruction &ins);
  reg_type get_val(uint32_t kindno, const Instruction &ins);
  // Set registers/memory/whatever. Yes, also has to be in header. Ugly.
  template <typename T>
  void set_val(ArgKind src, T val, const Instruction &ins) {
    // If this is a register, simply handoff to the register api
    if (is_reg(src)) {
      regs.set(src, val);
      return;
    }
    // Otherwise, probably gonna be memory.
    // Get the addr from the source
    addr addr = get_val(src, ins);
    this->mmu.write(addr, val);
  }
  MMU &get_mmu();
  Reg &get_regs();

private:
  Reg regs;
  Decoder decoder;
  MMU mmu;
  // For counting number of instructions executed.
  uint32_t num_insns = 0;
};
