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
} typedef Instruction;

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

  // Undefined instruction or reserved.
  NONE,
  // If opcode & 0xf == 0, it will either be CLR or a MOV variation. Idk how i
  // gonna do this anymo lol.
  CLR_OR_MOV_S0,
};
