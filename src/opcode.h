#pragma once
#include "instruction.h"
#include <array>
#include <cstdint>

const int max_opcodes = 0xff;
typedef uint8_t opcode_type;
typedef std::array<opcode_type, max_opcodes> op_array;

constexpr opcode_type as_op(InsFormat i) { return (opcode_type)i; }

namespace Opcodes {

// Fancy constexpr array evaluation so compiler can just stick this in rodata
// This is mainly encompassing stuff in the 3rd nibble which among other things
// denotes instruction type.
//
constexpr op_array op_arr = [] {
  op_array opcodes{0};

  // Movs:
  //

  // Adds:
  // Am, Dn
  opcodes[as_op(InsFormat::S0_DmDn_ADD)] = InsnType::ADD;
  // Dm, An
  opcodes[0b0110] = InsnType::ADD;
  // Am, An
  opcodes[0b0111] = InsnType::ADD;
  opcodes[as_op(InsFormat::S0_DmDn_ADD)] = InsnType::ADD;

  // Subs:
  // Am, Dn
  opcodes[0b0001] = InsnType::SUB;
  // Dm, An
  opcodes[0b0010] = InsnType::SUB;
  // Am, An
  opcodes[0b0011] = InsnType::SUB;

  // Nop
  opcodes[as_op(InsFormat::S0_NOP)] = InsnType::NOP;
  return opcodes;
}();

// Specifically for D* format opcodes, we will check the opcode and if we are
// above a certain threshold we will simply lookup in this table instead.
// Why? Just to save space, since if we were to include D* in the above, we
// would use up to an extra 8 bits (because they extend their opcode), meaning
// we would have to have a 0xffff size array. 2 0xff size arrays is better.
constexpr op_array dn_arr = [] {
  op_array dn_ops{0};

  // Adds:

  return dn_ops;
}();

}; // namespace Opcodes
