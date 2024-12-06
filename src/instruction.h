#pragma once
#include <cstdint>
typedef uint8_t inst_data;
typedef uint8_t inst_op;

struct Instruction {
  // Size of the current instruction in bytes.
  uint8_t sz;
  inst_op op;
  // May end up being 0 or more bytes depending on size of inst
  inst_data args[6];
};

// We have 11 possible formats
enum InsFormat {
  // D0 is moreso an umbrella, basically everything after the 5th bit may change
  // but the 5th will always be 0 for D0.
  D0 = 0b11110000,
  D1 = 0b11111000,
  D2 = 0b11111010,
  D4 = 0b11111100,
  D5 = 0b11111110,
  // D5 and D3 both share the same opcode format byte, and are differentiated
  // based on what comes after.
  D3 = 0b11111110,

};

enum InsnType {
  MOV,
  MOVBU,
  MOVB_1,
  MOVHU,
  MOVH_1,
  MOVM,

  EXT,
  EXTB,
  EXTH,
  EXTHU,

  CLR,

  ADD,
  ADDC,

  INC,
  INC4,

  SUB,
  SUBC,

  MUL,
  MULU,

  DIV,
  DIVU,

  CMP,

  OR,
  AND,
  NOT,
  XOR,

  BTST,

  BSET,

  BCLR,

  ASR_2,
  LSR_2,
  ASL,
  ASL2,

  ROR,
  ROL,

  Bcc,
  Lcc,
  JMP,

  SETLB,
  JSR_1,

  CALL,
  CALLS,
  TRAP,

  RET,
  RETF,
  RETS,
  RTS_1,
  RTI,

  NOP,

  UDFnn,
  UDFUnn,

};
