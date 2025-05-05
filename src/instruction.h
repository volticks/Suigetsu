#pragma once
#include "instruction.h"
#include <bit>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <netinet/in.h>
typedef uint8_t inst_data;
typedef uint8_t inst_op;

const uint32_t CARRY_BIT = 0x80000000;
const uint32_t min_ins = 1;
const uint32_t max_ins = 7;

// Some PSW bit meanings, TODO add more
enum PswBits {
  Z = 0b0001,
  N = 0b0010,
  C = 0b0100,
  V = 0b1000,
};

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
constexpr const char *insn_to_str(InsnType type) {
  switch (type) {
  case MOV:
    return "MOV";
  case MOVBU:
    return "MOVBU";
  case MOVHU:
    return "MOVHU";
  case MOVM:
    return "MOVM";
  case EXT:
    return "EXT";
  case EXTB:
    return "EXTB";
  case EXTBU:
    return "EXTBU";
  case EXTH:
    return "EXTH";
  case EXTHU:
    return "EXTHU";
  case CLR:
    return "CLR";
  case ADD:
    return "ADD";
  case ADDC:
    return "ADDC";
  case INC:
    return "INC";
  case INC4:
    return "INC4";
  case SUB:
    return "SUB";
  case SUBC:
    return "SUBC";
  case MUL:
    return "MUL";
  case MULU:
    return "MULU";
  case DIV:
    return "DIV";
  case DIVU:
    return "DIVU";
  case CMP:
    return "CMP";
  case OR:
    return "OR";
  case AND:
    return "AND";
  case NOT:
    return "NOT";
  case XOR:
    return "XOR";
  case BTST:
    return "BTST";
  case BSET:
    return "BSET";
  case BCLR:
    return "BCLR";
  case ASR:
    return "ASR";
  case LSR:
    return "LSR";
  case ASL:
    return "ASL";
  case ASL2:
    return "ASL2";
  case ROR:
    return "ROR";
  case ROL:
    return "ROL";
  case Bcc:
    return "Bcc";
  case Lcc:
    return "Lcc";
  case JMP:
    return "JMP";
  case JSR_1:
    return "JSR_1";
  case CALL:
    return "CALL";
  case CALLS:
    return "CALLS";
  case TRAP:
    return "TRAP";
  case RET:
    return "RET";
  case RETF:
    return "RETF";
  case RETS:
    return "RETS";
  case RTS_1:
    return "RTS_1";
  case RTI:
    return "RTI";
  case NOP:
    return "NOP";
  case UDFnn:
    return "UDFnn";
  case UDFUnn:
    return "UDFUnn";
  case NONE:
    return "NONE";
  case CLR_OR_MOV_S0:
    return "CLR_OR_MOV_S0";
  // Branch variants
  case BLT:
    return "BLT";
  case BGT:
    return "BGT";
  case BGE:
    return "BGE";
  case BLE:
    return "BLE";
  case BCS:
    return "BCS";
  case BHI:
    return "BHI";
  case BCC:
    return "BCC";
  case BLS:
    return "BLS";
  case BEQ:
    return "BEQ";
  case BNE:
    return "BNE";
  case BRA:
    return "BRA";
  // Loop instruction execution instructions
  case LLT:
    return "LLT";
  case LGT:
    return "LGT";
  case LGE:
    return "LGE";
  case LLE:
    return "LLE";
  case LCS:
    return "LCS";
  case LHI:
    return "LHI";
  case LCC:
    return "LCC";
  case LLS:
    return "LLS";
  case LEQ:
    return "LEQ";
  case LNE:
    return "LNE";
  case LRA:
    return "LRA";
  case SETLB:
    return "SETLB";
  case BVC:
    return "BVC";
  case BVS:
    return "BVS";
  case BNC:
    return "BNC";
  case BNS:
    return "BNS";
  default:
    return "UNKNOWN";
  }
}
constexpr const char *arg_kind_to_str(ArgKind arg) {
  switch (arg) {
  case ArgKind::D0:
    return "D0";
  case ArgKind::D1:
    return "D1";
  case ArgKind::D2:
    return "D2";
  case ArgKind::D3:
    return "D3";
  case ArgKind::A0:
    return "A0";
  case ArgKind::A1:
    return "A1";
  case ArgKind::A2:
    return "A2";
  case ArgKind::A3:
    return "A3";
  case ArgKind::PC:
    return "PC";
  case ArgKind::SP:
    return "SP";
  case ArgKind::MDR:
    return "MDR";
  case ArgKind::PSW:
    return "PSW";
  case ArgKind::LIR:
    return "LIR";
  case ArgKind::LAR:
    return "LAR";
  case ArgKind::imm8:
    return "imm8";
  case ArgKind::imm16:
    return "imm16";
  case ArgKind::imm32:
    return "imm32";
  case ArgKind::imm40:
    return "imm40";
  case ArgKind::imm48:
    return "imm48";
  case ArgKind::regs:
    return "regs";
  case ArgKind::abs16:
    return "abs16";
  case ArgKind::abs32:
    return "abs32";
  case ArgKind::d8:
    return "d8";
  case ArgKind::d16:
    return "d16";
  case ArgKind::d32:
    return "d32";
  case ArgKind::MA0:
    return "MA0";
  case ArgKind::MA1:
    return "MA1";
  case ArgKind::MA2:
    return "MA2";
  case ArgKind::MA3:
    return "MA3";
  case ArgKind::MD0:
    return "MD0";
  case ArgKind::MD1:
    return "MD1";
  case ArgKind::MD2:
    return "MD2";
  case ArgKind::MD3:
    return "MD3";
  default:
    return "UNKNOWN";
  }
}

// Is our argument data?
constexpr bool arg_isdata(ArgKind kind) {
  return kind >= ArgKind::imm8 && kind <= ArgKind::d32 || kind == ArgKind::regs;
}

constexpr uint32_t get_arg_sz(ArgKind kind) {
  uint32_t sz = 0;
  if (!arg_isdata(kind))
    return sz;
  switch (kind) {
  case ArgKind::regs:
  case ArgKind::imm8:
  case ArgKind::d8:
    sz = 1;
    break;
  case ArgKind::imm16:
  case ArgKind::d16:
  case ArgKind::abs16:
    sz = 2;
    break;
  case ArgKind::imm32:
  case ArgKind::d32:
  case ArgKind::abs32:
    sz = 4;
    break;
  case ArgKind::imm40:
    sz = 5;
    break;
  case ArgKind::imm48:
    sz = 6;
    break;
  }
  return sz;
};
// TODO: Remove these
// Convert parts of the instruction type enum to a string
// const char *insn_to_str(InsnType type);
// Converts arg kind enum to string
// const char *arg_kind_to_str(ArgKind arg);
// Test if arg is data
// constexpr bool arg_isdata(ArgKind kind);
// Retrieve the size of the (data) argkind in bytes.
// 0 on non-data arg.
// constexpr uint32_t get_arg_sz(ArgKind kind);

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
          d_idx -= 1;
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

  uint32_t get_arg(uint32_t kindno) const {
    static const uint32_t end = sizeof(this->args) / sizeof(this->args[0]);
    assert(kindno < end);

    const ArgKind src = this->kinds[kindno];
    uint32_t s = 0;

    uint32_t idx = 0;
    if (kindno > 0) {
      // Need to add sizes of other kinds
      idx += get_arg_sz(this->kinds[0]);
      if (kindno > 1)
        idx += get_arg_sz(this->kinds[1]);
    }

    // TODO: extend for abs where appropriate?
    uint8_t sz = get_arg_sz(src);

    // TODO: Im a lazy bastard who writes asserts instead of throwing exceptions
    assert(idx + sz <= end && this->curr >= idx + sz);
    uint8_t curr = 0;
    for (int i = idx + sz; i > idx && i > 0; i--) {
      curr = this->args[this->curr - i];
      s += curr << (8 * ((idx + sz) - i));
    }

    return s;
  }

}; // typedef Instruction;
