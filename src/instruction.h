#pragma once
#include "instruction.h"
#include "registers.h"
#include <cstdint>
#include <cstdio>
#include <iostream>
typedef uint8_t inst_data;
typedef uint8_t inst_op;

// TODO: add Dn aswell
enum InsSzSn {
  S0 = 1,
  S1 = 2,
  S2 = 3,
  S3 = 4,
  S4 = 5,
  S5 = 6, // Dont think this exists?
  S6 = 7,
};

// Non-zero ArgKind in an instruction == populated.
// Otherwise we reached end of args.
enum class ArgKind {
  NONE = 0,
  D0, // = 1,
  D1,
  D2,
  D3,
  A0,
  A1,
  A2,
  A3,
  PC,
  SP,
  MDR,
  PSW,
  LIR,
  LAR,

  imm8,
  imm16,
  imm32,
  imm40,
  imm48,

  // 8 bit value representing a collection of registers?
  regs,

  // Absolute values, exclusively used for memory related stuff
  abs16,
  abs32,

  // Displacements, used for relative jumps and stuff.
  d8,
  d16,
  d32,

  // Memory versions of the registers.
  MA0,
  MA1,
  MA2,
  MA3,
  //// Dont believe Dn has the same but just in case
  MD0,
  MD1,
  MD2,
  MD3,

};
// Key:
// - Camelcase m before register name: dereference of ptr in reg
// - Instruction name after: We prolly had a duplicate so just naming it
// something else lol. We have 11 possible formats
enum class InsFormat {
  // D0 is moreso an umbrella, basically everything after the 5th bit may change
  // but the 5th will always be 0 for D0.
  D0 = 0b11110000,
  D0_1 = 0b11110010,
  D0_2 = 0b11110011,
  D1 = 0b11111000,
  D2 = 0b11111010,
  D4 = 0b11111100,
  D5 = 0b11111110,
  // D5 and D3 both share the same opcode format byte, and are differentiated
  // based on what comes after.
  D3 = 0b11111110,

  // Whereas Dn all start with 1111, Sn is anything thats doesnt. Also, Sn
  // format only has the first 4 bits as an opcode. This kinda sucks cuz it
  // means documenting it in a enum is kinda
  // ass but we ball.
  //
  // S0 and variations
  //
  //// S0 Mov
  S0_DmDn_MOV = 0b1000,
  S0_AmAn = 0b1001,
  S0_SpAn = 0b0011,
  // Deref of Am
  S0_mAmDn = 0b0111,
  // Deref of An
  S0_Dm_mAn = 0b0111,
  S0_Dn = 0b0001,
  //// End S0 Mov
  // Only ever used for clr'ing Dn I believe
  S0_Dn_CLR = 0b0000,
  //// S0 Add
  S0_DmDn_ADD = 0b1110,
  //// S0 INC
  // All inc share S0 and share the same opcode, so just gotta read after the
  // opcode.
  S0_Dn_INC = 0b0100,
  //// S0 INC4
  S0_An_INC4 = 0b0101,
  //// S0 CMP
  S0_DmDn_CMP = 0b1010,
  S0_AmAn_CMP = 0b1011,
  //// S0 ASL2
  // Same as INC4, but differentiated based on what comes
  // after. INC4 has 00 bits, we have 01.
  S0_Dn_ASL2 = 0b0101,
  //// S0 Lcc
  // For LEQ last 4 bits are 1000, for LNE its 1001.
  S0_Lcc = 0b1101,
  //// S0 SETLB, last nib is 1011
  S0_SETLB = 0b1101,
  //// S0 NOP
  S0_NOP = 0b1100,

  //
  // S1 and variations
  //
  S2_i16_Dn_ADD = 0b001010,
  S2_i16_An_ADD = 0b001000,

  //
  // S2 and variations
  //
  S2_i16_Dn_MOV = 0b001011,
  S2_i16_An_MOV = 0b001001,

  //
  // S4 and variations
  //

  //
  // S6 and variations
  //

  //
  // D0 and variations
  //
  // Adds:
  D0_DmAn_ADD = 0b00010101,

  //
  // D1 and variations
  //

  //
  // D2 and variations
  //

  //
  // D3 and variations
  //

  //
  // D4 and variations
  //

  //
  // D5 and variations
  //

};

enum InsnType {
  MOV,
  MOVBU,
  MOVHU,
  MOVM,

  EXT,
  EXTB,
  EXTBU,
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

  // Undefined instruction or reserved.
  NONE,
  // If opcode & 0xf == 0, it will either be CLR or a MOV variation. Idk how i
  // gonna do this anymo lol.
  CLR_OR_MOV_S0,

  // Branch variants - order is important
  BLT,
  BGT,
  BGE,
  BLE,
  BCS,
  BHI,
  BCC,
  BLS,
  BEQ,
  BNE,
  BRA,

  // Loop instruction execution instructions
  // - order important
  LLT,
  LGT,
  LGE,
  LLE,
  LCS,
  LHI,
  LCC,
  LLS,
  LEQ,
  LNE,
  LRA,
  SETLB,

};

// Convert parts of the instruction type enum to a string
const char *insn_to_str(InsnType type);
// Converts arg kind enum to string
const char *arg_kind_to_str(ArgKind arg);

struct Instruction {
  // Size of the current instruction in bytes.
  uint8_t sz;
  inst_op op;
  // Specifies the kinds of arguments, in order.
  ArgKind kinds[3];
  // May end up being 0 or more bytes depending on size of inst. This is mainly
  // here so we can store the imm/abs/displacment values
  inst_data args[6];

  uint8_t curr = 0;
  // Some helpers
  // Appending an argument to the array. Can have up to 6 bytes of arguments so
  // wanted a way to manage this. This needs to be in here for some reason,
  // putting it in an external cc file aint workin.
  bool arg_add(const inst_data *const arg, uint32_t len) {
    if (this->curr + len >= sizeof(this->args))
      return false;

    for (int i = 0; i < len; i++) {
      this->args[curr + i] = arg[i];
    }

    this->curr += len;
    return true;
  }
  void log() {

    std::cout << insn_to_str((InsnType)this->op) << " ";
    if (this->kinds[0] != ArgKind::NONE)
      std::cout << arg_kind_to_str(this->kinds[0]);
    if (this->kinds[1] != ArgKind::NONE)
      std::cout << " " << arg_kind_to_str(this->kinds[1]);
    std::cout << std::endl;

    fprintf(stdout, "ARGS: %s\n", this->args);
  }

}; // typedef Instruction;
