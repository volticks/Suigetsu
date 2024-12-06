#pragma once
#include "instruction.h"
#include <array>
#include <cstdint>
const int max_opcodes = 0xff;
typedef uint8_t opcode_type;
typedef std::array<opcode_type, max_opcodes> op_array;

namespace Opcodes {

// Fancy constexpr array evaluation so compiler can just stick this in rodata
// This is mainly encompassing stuff in the 3rd nibble which among other things
// denotes instruction type.
//
constexpr op_array op_arr = [] {
  op_array opcodes{0};

  // Adds:
  // Am, Dn
  opcodes[0b0101] = InsnType::ADD;
  // Dm, An
  opcodes[0b0110] = InsnType::ADD;
  // Am, An
  opcodes[0b0111] = InsnType::ADD;

  // Subs:
  // Am, Dn
  opcodes[0b0001] = InsnType::SUB;
  // Dm, An
  opcodes[0b0001] = InsnType::SUB;
  // Am, An
  opcodes[0b0001] = InsnType::SUB;
  return opcodes;
}();

}; // namespace Opcodes
