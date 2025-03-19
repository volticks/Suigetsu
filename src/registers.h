#pragma once
#include "instruction.h"
#include <cstdint>
#include <exception>
typedef uint32_t reg_type;
typedef int32_t reg_type_s;
typedef uint32_t reg_idx;
// const int num_reg = 4 + 3 + 6;
const uint32_t num_reg = static_cast<uint32_t>(ArgKind::MA3);
const ArgKind reg_usable = ArgKind::MA3;
const ArgKind mem_reg_off = ArgKind::SP;

// Should be able to index into the array with these enums
// enum RegTypes {
//  D0,
//  D1,
//  D2,
//  D3,
//  A0,
//  A1,
//  A2,
//  A3,
//  SP,
//  PC,
//  MDR,
//  PSW, // 16bits
//  LIR,
//  LAR,
//  // Memory versions of the registers.
//  MA0,
//  MA1,
//  MA2,
//  MA3,
//  //// Dont believe Dn has the same but just in case
//  MD0,
//  MD1,
//  MD2,
//  MD3,
//
//};
// Convert reg enum to string
const char *reg_to_str(ArgKind reg);
// Is the supplied kind a register?

class Reg {
public:
  Reg();

  // specialised functions cuz we prolly gonna be using these A LOT
  reg_type get_pc();
  void set_pc(reg_type pc);

  // general register access
  reg_type get(ArgKind reg, bool logging = true);
  bool set(ArgKind reg, reg_type val);

  void dump_regs();

private:
  reg_type registers[num_reg];
};

class RegisterException : public std::exception {
public:
  RegisterException(char *newMsg) : msg(newMsg) {}

  const char *what() const noexcept { return msg; }

private:
  char *msg;
};
