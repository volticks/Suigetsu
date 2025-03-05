#pragma once
#include "instruction.h"
#include <cstdint>
#include <cstdio>
#include <iostream>
typedef uint8_t inst_data;
typedef uint8_t inst_op;

const uint32_t CARRY_BIT = 0x80000000;

// Some PSW bit meanings, TODO add more
enum PswBits {
  Z = 0b0001,
  N = 0b0010,
  C = 0b0100,
  V = 0b1000,
};

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

enum InsSzDn {
  D0s = 2,
  D1s = 3,
  D2s = 4,
  D3s = 5,
  D4s = 6,
  D5s = 7,
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

  // Data args, grouped together - order important
  imm8,
  imm16,
  imm32,
  imm40,
  imm48,
  // Absolute values, exclusively used for memory related stuff
  abs16,
  abs32,
  // 8 bit value representing a collection of registers?
  regs,
  // Displacements, used for relative jumps and stuff.
  d8,
  d16,
  d32,

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

  // Bit testing - order matters
  BSET,
  BCLR,
  BTST,

  ASR_2,
  LSR_2,
  ASL,
  ASL2,
  ASR,
  LSR,

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

  // Branch instructions - order important
  BVC,
  BVS,
  BNC,
  BNS,
};

// Convert parts of the instruction type enum to a string
const char *insn_to_str(InsnType type);
// Converts arg kind enum to string
const char *arg_kind_to_str(ArgKind arg);
// Test if arg is data
bool arg_isdata(ArgKind kind);
// Retrieve the size of the (data) argkind in bytes.
// 0 on non-data arg.
uint32_t get_arg_sz(ArgKind kind);

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
    if (this->curr + len > (sizeof(this->args) / sizeof(this->args[0])))
      return false;

    for (int i = 0; i < len; i++) {
      this->args[curr + i] = arg[i];
    }

    this->curr += len;
    return true;
  }

  void copy_data_args(const inst_data *const args, uint32_t len) {
    // Data args are in reverse order starting at the end of the instruction, so
    // need to start from the end. Data idx
    // int d_idx = len;
    int d_idx = 0;
    const int kind_sz = sizeof(this->kinds) / sizeof(kinds[0]);
    uint32_t i = 0;
    int kind_idx = kind_sz;
    int j = 0;
    ArgKind kind;

    std::cout << insn_to_str((InsnType)this->op) << " ";

    while (kind_idx >= 0) {
      kind = this->kinds[kind_idx];
      kind_idx--;

      if (!arg_isdata(kind))
        continue;

      int arg_sz = get_arg_sz(kind);
      int end = i + arg_sz;

      if (end > len) {
        std::cerr << "Instruction::copy_data_args went oob" << std::endl;
        return;
      }

      int d_start = d_idx; // - arg_sz;
      std::cout << "Instruction::copy_data_args curr arg: " << args[d_start]
                << " size: " << arg_sz << std::endl;

      this->arg_add(&args[d_start], arg_sz);
      i = end;
      // d_idx -= arg_sz;
      d_idx += arg_sz;
    }
  }
  void log() {

    const int kind_sz = sizeof(this->kinds) / sizeof(kinds[0]);
    uint32_t i = 0;
    // Data idx
    int d_idx = this->curr - 1;
    int j = 0;
    ArgKind kind;

    std::cout << "\t\t" << insn_to_str((InsnType)this->op) << " ";

    while (i < kind_sz && this->kinds[i] != ArgKind::NONE) {
      if (i)
        std::cout << ", ";

      kind = this->kinds[i];

      if (!arg_isdata(kind)) {
        std::cout << arg_kind_to_str(kind);
      } else {

        // TODO: decoding and logging of registers in these things???
        if (kind == ArgKind::regs) {
          std::cout << "regs";
          i++;
          continue;
        }

        int arg_sz = get_arg_sz(kind);
        int sz_cpy = arg_sz;
        bool found_nz = false;
        for (j = d_idx; j >= 0 && arg_sz > 0; j--, arg_sz--) {

          if (!found_nz && this->args[j] == 0 && sz_cpy > 1 && !(arg_sz == 1))
            continue;
          else
            found_nz = true;

          if (this->args[j] == 0)
            std::cout << "0";
          std::cout << std::hex << (int)this->args[j];
        }
        d_idx -= sz_cpy;
      }
      i++;
    }
    std::cout << std::endl;
  }

}; // typedef Instruction;
