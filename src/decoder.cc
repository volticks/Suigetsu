#include "decoder.h"
#include "instruction.h"
#include "opcode.h"
#include "registers.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <ostream>

static inline inst_data nib_up(inst_op op) { return (op & 0xf0) >> 4; }
static inline inst_data nib_low(inst_op op) { return (op & 0xf); }
// Handler function for Sn ops
static void handle_sn(inst_data *data, Instruction &ins_out) {}

const uint8_t dn_idx = 1;
const uint8_t an_idx = 0;
const ArgKind *regs[] = {an_registers, dn_registers};

// Sn has a very limited number of instructions it could be.
// This is basically just pseudocode for now, need to work on this big time.
// Want to use the opcodes array for something since i went to all that trouble
// but not sure i'll find a use for it in the end cuz this part switch part
// checking is ugly asf.
// TODO: Something about this.
// Should we do some kind of masking of the bits here or something? Maybe have a
// mask for the CLR instruction we can and the opcode against, since we know
// that if the value is 0,4,8,or 0xC then we have a mask which checks for these
// bits specifically, this also means we wont be just writing the instruction
// set entirely by hand into our program lol. Just theorycrafting anywho.
void Decoder::decode_sn_op(const inst_data *data, Instruction &ins_out) {
  inst_op op = *data;
  inst_op op_nib_up = nib_up(op);
  inst_op op_nib_low = nib_low(op);
  ArgKind reg;
  bool use_d;
  uint8_t idx;
  this->add_args = false;
  this->arg_sz = 0;
  // TODO: use this
  switch (op_nib_up) {
  case 0x0: {
    std::cout << "Decoder::decode_sn_op entering op_nib_up < 1 branch"
              << std::endl;
    // For Sn, this will either be a CLR or a mov variation with the Dn
    // registers. Lets eliminate the former first.
    const inst_op mask_clr = 0b1100;
    reg = dn_registers[(op_nib_low & mask_clr) >> 2];
    ins_out.kinds[0] = reg;
    idx = op_nib_low % 4;
    if (idx == 0) {
      // Issa CLR. We know it'll be on one of the Dn registers
      // Then need to store it on the instruction.
      std::cout << "Decoder::decode_sn_op instruction is CLR" << std::endl;
      ins_out.op = InsnType::CLR;
      ins_out.sz = InsSzSn::S0;
      break;
    }

    // mov/hu/bu Dn,(abs16)
    const inst_data movhu_sn_upper = 0xf;
    const inst_data movbu_sn_upper = 0xe;
    const inst_data mov_sn_upper = 0xd;

    ins_out.kinds[1] = ArgKind::abs16;
    ins_out.sz = InsSzSn::S2;

    if ((movhu_sn_upper - op) % 0x4 == 0) {
      // Probably a movhu inst
      std::cout << "Decoder::decode_sn_op instruction is MOVHU" << std::endl;
      ins_out.op = InsnType::MOVHU;
    } else if ((movbu_sn_upper - op) % 0x4 == 0) {
      // Probably a movbu
      std::cout << "Decoder::decode_sn_op instruction is MOVBU" << std::endl;
      ins_out.op = InsnType::MOVBU;
    } else {
      // Gotta be a plain mov
      std::cout << "Decoder::decode_sn_op instruction is MOV" << std::endl;
      ins_out.op = InsnType::MOV;
    }
    // All the above have abs16 for second.
    // Need +1 to skip past the opcode
    add_args = true;
    arg_sz = 2;

    break;
  }
  case 0x1: {
    // TODO: BIG: Test decoding these instructions branches below.
    const InsnType types[] = {EXTB, EXTBU, EXTH, EXTHU};
    // Handles EXTB/BU/H/HU variants
    ins_out.sz = InsSzSn::S0;
    // Didnt feel like sketching out a whole ass if else chain.
    // Will be max 3
    ins_out.op = types[op_nib_low >> 2];
    ArgKind reg = dn_registers[op_nib_low & 0b0011];
    ins_out.kinds[0] = reg;

    break;
  }
  case 0x2: {
    // Mov or add variation with a imm8 or imm16
    // const InsnType types[] = {ADD, MOV};
    ins_out.op = ((op_nib_low % 8) > 3) ? MOV : ADD;
    std::cout << "Decoder::decode_sn_op instruction is ";
    if (ins_out.op == MOV) {
      std::cout << "\"MOV imm16, ";
      ins_out.kinds[0] = ArgKind::imm16;
      ins_out.sz = InsSzSn::S2;
    } else {
      std::cout << "\"ADD imm8, ";
      ins_out.kinds[0] = ArgKind::imm8;
      ins_out.sz = InsSzSn::S1;
    }

    use_d = op_nib_low & 0b10;
    idx = (op_nib_low & 0b0011);
    if (use_d) {
      std::cout << "d" << (int)idx << "\"" << std::endl;
      reg = dn_registers[idx];
    } else {
      std::cout << "a" << (int)idx << std::endl;
      reg = an_registers[idx];
    }
    ins_out.kinds[1] = reg;
    add_args = true;
    arg_sz = ((op_nib_low % 8) > 3) ? 2 : 1;

    break;
  }
  case 0x3: {
    // TODO: Test
    // Mov/b/hu variation with either SP or abs16
    const InsnType types[] = {MOV, MOVBU, MOVHU, MOV};
    idx = op_nib_low & 0b11;
    ins_out.op = types[idx];
    if (idx < 3) {
      reg = dn_registers[idx];
      ins_out.kinds[0] = ArgKind::abs16;
      add_args = true;
      arg_sz = 2;
      ins_out.sz = InsSzSn::S3;
    } else {
      reg = an_registers[idx];
      ins_out.kinds[0] = ArgKind::SP;
      ins_out.sz = InsSzSn::S0;
    }
    ins_out.kinds[1] = reg;

    break;
  }
  case 0x4: {
    // TODO: Test
    // Inc or mov variation
    const InsnType types[] = {INC, INC, MOV, MOV};
    idx = op_nib_low & 0b11;

    ins_out.op = types[idx];
    use_d = !(idx & 0b01);
    ins_out.kinds[0] = regs[use_d][idx];
    if (idx & 0b10) {
      // Need to set kinds[1] and set size
      // MOV (D|A)n,(d8,SP)
      ins_out.kinds[1] = ArgKind::d8;
      ins_out.kinds[2] = ArgKind::SP;
      ins_out.sz = InsSzSn::S1;
    } else {
      ins_out.sz = InsSzSn::S0;
    }
    break;
  }
  case 0x5: {
    // TODO: Test
    // INC, ASL, or mov variation
    const InsnType types[] = {INC4, ASL2, MOV, MOV};
    idx = op_nib_low >> 2;
    ins_out.op = types[idx];

    use_d = (op_nib_low % 0x8) < 4;
    if (op_nib_low > 0x7) {
      ins_out.kinds[0] = regs[use_d][op_nib_low % 4];
      ins_out.kinds[1] = ArgKind::d8;
      ins_out.kinds[2] = ArgKind::SP;
      ins_out.sz = InsSzSn::S1;
      add_args = true;
      arg_sz = 1;
    } else {
      ins_out.kinds[0] = regs[(!use_d)][op_nib_low % 4];
      ins_out.sz = InsSzSn::S0;
    }
    break;
  }
  case 0x6: {
    // TODO: Test
    // TODO: Need some way to
    // This is all mov dm, (an)
    ins_out.op = MOV;
    ins_out.sz = InsSzSn::S0;
    ins_out.kinds[0] = regs[dn_idx][(op_nib_low & 0b1100) >> 2];
    ins_out.kinds[1] = regs[an_idx][(op_nib_low & 0b0011) + mem_id];

    break;
  }
  case 0x7: {
    // TODO: Test
    // This is all mov (am), dn
    ins_out.op = MOV;
    ins_out.sz = InsSzSn::S0;
    ins_out.kinds[0] = regs[an_idx][(op_nib_low & 0b0011) + mem_id];
    ins_out.kinds[1] = regs[dn_idx][(op_nib_low & 0b1100) >> 2];
    break;
  }
  case 0x8: {
    // TODO: Test
    // Mov Dm, Dn
    ins_out.op = MOV;
    ins_out.kinds[1] = regs[dn_idx][op_nib_low & 0b0011];
    if (((op_nib_low & 0b1100) >> 2) == (op_nib_low & 0b0011)) {
      // Needs to be converted to mov imm8, dn
      ins_out.sz = InsSzSn::S1;
      ins_out.kinds[0] = ArgKind::imm8;
      add_args = true;
      arg_sz = 1;
      break;
    }
    ins_out.sz = InsSzSn::S0;
    ins_out.kinds[0] = regs[dn_idx][(op_nib_low & 0b1100) >> 2];
    break;
  }
  case 0x9: {
    // TODO: Test
    // Mov Am, An
    ins_out.op = MOV;
    ins_out.kinds[1] = regs[an_idx][op_nib_low & 0b0011];
    if (((op_nib_low & 0b1100) >> 2) == (op_nib_low & 0b0011)) {
      // Mov imm8, an
      ins_out.kinds[0] = ArgKind::imm8;
      ins_out.sz = InsSzSn::S1;
      add_args = true;
      arg_sz = 1;
      break;
    }
    ins_out.sz = InsSzSn::S0;
    ins_out.kinds[0] = regs[an_idx][(op_nib_low & 0b1100) >> 2];
    break;
  }
  case 0xa: {
    // TODO: Test
    // Cmp Dm, Dn
    ins_out.op = CMP;
    ins_out.kinds[1] = regs[dn_idx][(op_nib_low & 0b1100) >> 2];
    if (((op_nib_low & 0b1100) >> 2) == (op_nib_low & 0b0011)) {
      // Cmp imm8, Dn
      ins_out.kinds[0] = ArgKind::imm8;
      ins_out.sz = InsSzSn::S1;
      add_args = true;
      arg_sz = 1;
      break;
    }
    ins_out.sz = InsSzSn::S0;
    ins_out.kinds[0] = regs[dn_idx][(op_nib_low & 0b1100) >> 2];
    break;
  }
  case 0xb: {
    // TODO: Test
    // Cmp Am, An
    ins_out.op = CMP;
    ins_out.kinds[1] = regs[an_idx][(op_nib_low & 0b1100) >> 2];
    if (((op_nib_low & 0b1100) >> 2) == (op_nib_low & 0b0011)) {
      // Cmp imm8, An
      ins_out.kinds[0] = ArgKind::imm8;
      ins_out.sz = InsSzSn::S1;
      add_args = true;
      arg_sz = 1;
      break;
    }
    ins_out.sz = InsSzSn::S0;
    ins_out.kinds[0] = regs[an_idx][(op_nib_low & 0b1100) >> 2];
    break;
  }
  case 0xc: {
    // TODO: Test
    // This one is a creature
    if (op_nib_low == 0xb) {
      // Handle NOP
      ins_out.op = NOP;
      ins_out.sz = InsSzSn::S0;
      break;
    }

    add_args = true;
    arg_sz = 1;
    const InsnType types[] = {JMP, CALL, MOVM, MOVM};
    if (op_nib_low < 0xc) {
      // Branch variant
      ins_out.op = InsnType::BLT + op_nib_low;
      ins_out.sz = InsSzSn::S1;
      ins_out.kinds[0] = ArgKind::d8;
      ins_out.kinds[1] = ArgKind::PC;
      break;
    }

    idx = op_nib_low - 0xc;
    ins_out.op = types[idx];

    if (idx < 2) {
      arg_sz = 2;
      ins_out.sz = InsSzSn::S2;
      ins_out.kinds[0] = ArgKind::d16;
      ins_out.kinds[1] = ArgKind::PC;
      // TODO
    } else {
      // TODO
      ins_out.sz = InsSzSn::S1;
      ArgKind one = (idx == 2) ? ArgKind::SP : ArgKind::regs;
      ArgKind two = (idx == 2) ? ArgKind::regs : ArgKind::SP;
      ins_out.kinds[0] = one;
      ins_out.kinds[1] = two;
    }
    // TODO
    break;
  }
  case 0xd: {
    // TODO: Test
    // Also a creature
    const InsnType types[] = {JMP, CALL, RETF, RET};
    if (op_nib_low < 0xc) {
      // Branch variant
      ins_out.op = InsnType::LLT + op_nib_low;
      ins_out.sz = InsSzSn::S0;
      break;
    }

    idx = op_nib_low % 4;
    ins_out.op = types[idx];
    if (idx > 2) {
      ins_out.sz = InsSzSn::S0;
      break;
    }

    ins_out.kinds[0] = ArgKind::d32;
    ins_out.kinds[1] = ArgKind::PC;
    ins_out.sz = InsSzSn::S4;
    add_args = true;
    arg_sz = 4;

    break;
  }
  case 0xe: {
    // TODO: Test
    // Add Dm, Dn
    ins_out.op = ADD;
    ins_out.sz = InsSzSn::S0;
    ins_out.kinds[0] = regs[dn_idx][(op_nib_low & 0b1100) >> 2];
    ins_out.kinds[1] = regs[dn_idx][op_nib_low & 0b0011];
    break;
  }
  case 0xf: {
    // TODO: Test
    // ???
    break;
  }
  default: {
    std::cerr << "Decoder::decode_sn_op wtf opcode, upper nib: " << op_nib_up
              << std::endl;
    break;
  }
  }
  if (add_args && arg_sz) {
    data++;
    if (data + arg_sz >= this->end) {
      // Cant decode if we go oob
      ins_out.sz = 0;
      arg_sz = 0;
    }
    ins_out.arg_add(data, arg_sz);
  }

  ins_out.log();
}
// Believe for some reason the memory operand always comes last
// Could also be that Dn always comes before the An register encoding
void Decoder::decode_dn_op_F0(const inst_data *data, Instruction &ins) {
  ++data;
  const inst_op op = *data;
  const inst_op op_nib_up = nib_up(op);
  const inst_op op_nib_low = nib_low(op);
  ArgKind reg;
  bool use_d;
  uint8_t idx;
  switch (op_nib_up) {
  case 0x0: {
    // TODO: Test
    // Mov (am), an
    ins.op = MOV;
    ins.sz = InsSzDn::D0s;
    ins.kinds[0] = regs[an_idx][(op_nib_low & 0b0011) + mem_id];
    ins.kinds[1] = regs[an_idx][(op_nib_low & 0b1100) >> 2];
    break;
  }
  case 0x1: {
    // TODO: Test
    // Mov am, (an)
    ins.op = MOV;
    ins.sz = InsSzDn::D0s;
    ins.kinds[0] = regs[an_idx][(op_nib_low & 0b1100) >> 2];
    ins.kinds[1] = regs[an_idx][(op_nib_low & 0b0011) + mem_id];
    break;
  }
  case 0x2: {
    // TODO: Test
    ins.op = NONE;
    ins.sz = InsSzDn::D0s;
    break;
  }
  case 0x3: {
    // TODO: Test
    ins.op = NONE;
    ins.sz = InsSzDn::D0s;
    break;
  }
  case 0x4: {
    // TODO: Test
    // movbu (am), dn
    ins.op = MOVBU;
    ins.sz = InsSzDn::D0s;
    ins.kinds[0] = regs[an_idx][(op_nib_low & 0b0011) + mem_id];
    ins.kinds[1] = regs[dn_idx][(op_nib_low & 0b1100) >> 2];

    break;
  }
  case 0x5: {
    // TODO: Test
    // movbu dm, (an)
    ins.op = MOVBU;
    ins.sz = InsSzDn::D0s;
    ins.kinds[0] = regs[dn_idx][(op_nib_low & 0b1100) >> 2];
    ins.kinds[1] = regs[an_idx][(op_nib_low & 0b0011) + mem_id];
    break;
  }
  case 0x6: {
    // TODO: Test

    // movhu (am), dn
    ins.op = MOVHU;
    ins.sz = InsSzDn::D0s;
    ins.kinds[0] = regs[an_idx][(op_nib_low & 0b0011) + mem_id];
    ins.kinds[1] = regs[dn_idx][(op_nib_low & 0b1100) >> 2];
    break;
  }
  case 0x7: {
    // TODO: Test
    // movhu dm, (an)
    ins.op = MOVHU;
    ins.sz = InsSzDn::D0s;
    ins.kinds[0] = regs[dn_idx][(op_nib_low & 0b1100) >> 2];
    ins.kinds[1] = regs[an_idx][(op_nib_low & 0b0011) + mem_id];
    break;
  }
  case 0x8: {
    // TODO: Test
    // bset dm, (an)
    ins.op = BSET;
    ins.sz = InsSzDn::D0s;
    ins.kinds[0] = regs[dn_idx][(op_nib_low & 0b1100) >> 2];
    ins.kinds[1] = regs[an_idx][(op_nib_low & 0b0011) + mem_id];
    break;
  }
  case 0x9: {
    // TODO: Test
    // bclr dm, (an)
    ins.op = BCLR;
    ins.sz = InsSzDn::D0s;
    ins.kinds[0] = regs[dn_idx][(op_nib_low & 0b1100) >> 2];
    ins.kinds[1] = regs[an_idx][(op_nib_low & 0b0011) + mem_id];
    break;
  }
  case 0xA: {
    // TODO: Test
    ins.op = NONE;
    ins.sz = InsSzDn::D0s;
    break;
  }
  case 0xB: {
    // TODO: Test
    ins.op = NONE;
    ins.sz = InsSzDn::D0s;
    break;
  }
  case 0xC: {
    // TODO: Test
    ins.op = NONE;
    ins.sz = InsSzDn::D0s;
    break;
  }
  case 0xD: {
    // TODO: Test
    ins.op = NONE;
    ins.sz = InsSzDn::D0s;
    break;
  }
  case 0xE: {
    // TODO: Test
    ins.op = NONE;
    ins.sz = InsSzDn::D0s;
    break;
  }
  case 0xF: {
    // TODO: Test
    ins.op = NONE;
    ins.sz = InsSzDn::D0s;
    if (op_nib_low > 7) {
      if (op_nib_low < 0xc || op_nib_low == 0xf) {
        // Unused
        ins.op = NONE;
        ins.sz = InsSzDn::D0s;
        break;
      }
      const InsnType types[] = {RETS, RTI, TRAP};
      idx = op_nib_low % 3;
      ins.op = types[idx];
      break;
    }

    // Either calls or jmp
    ins.op = (op_nib_low > 3) ? JMP : CALLS;
    ins.kinds[0] = regs[an_idx][(op_nib_low & 0b0011) + mem_id];
    break;
  }
  }
}

void Decoder::decode_dn_op_F1(const inst_data *data, Instruction &ins) {
  ++data;
  const inst_op op = *data;
  const inst_op op_nib_up = nib_up(op);
  const inst_op op_nib_low = nib_low(op);
  bool use_d;
  uint8_t i_r0;
  uint8_t i_r1;

  ins.sz = InsSzDn::D0s;
  const uint8_t reg_low = ((op_nib_low & 0b1100) >> 2);
  const uint8_t reg_high = ((op_nib_low & 0b0011));
  // Order is
  // Dm, Dn
  // Am, Dm
  // Dm, An
  // Am, An
  const uint8_t order[][2] = {
      {dn_idx, dn_idx}, {an_idx, dn_idx}, {dn_idx, an_idx}, {an_idx, an_idx}};

  i_r0 = order[op_nib_up % 4][0];
  i_r1 = order[op_nib_up % 4][1];
  ins.kinds[0] = regs[i_r0][reg_low];
  ins.kinds[1] = regs[i_r1][reg_high];
  switch (op_nib_up) {
  case 0x0:
  case 0x1:
  case 0x2:
  case 0x3:
    ins.op = SUB;
    break;
  case 0x4:
    ins.op = ADDC;
    break;
  case 0x5:
  case 0x6:
  case 0x7: {
    ins.op = ADD;
    // Order is consistent in this case
    // if (op_nib_up == 0x7)
    //  break;
    //// Switch order of registers
    // ArgKind tmp = ins.kinds[0];
    // ins.kinds[0] = ins.kinds[1];
    // ins.kinds[1] = tmp;
    break;
  }
  case 0x8: {
    ins.op = SUBC;
    break;
  }
  case 0x9:
  case 0xA:
    ins.op = CMP;
    break;
  case 0xB:
  case 0xC: {
    ins.op = NONE;
    break;
  }
  case 0xD:
  case 0xE:
    ins.op = MOV;
    break;
  case 0xF: {
    break;
  }
  }
}

void Decoder::decode_dn_op_F2(const inst_data *data, Instruction &ins) {
  ++data;
  const inst_op op = *data;
  const inst_op op_nib_up = nib_up(op);
  const inst_op op_nib_low = nib_low(op);
  ArgKind reg;
  bool use_d;
  uint8_t idx;

  ins.sz = InsSzDn::D0s;

  const uint8_t reg_low = ((op_nib_low & 0b1100) >> 2);
  const uint8_t reg_high = ((op_nib_low & 0b0011));
  // Same for *most* of these, some require manual changes
  ins.kinds[0] = regs[dn_idx][reg_low];
  ins.kinds[1] = regs[dn_idx][reg_high];

  switch (op_nib_up) {
  case 0x0: {
    ins.op = AND;
    break;
  }
  case 0x1: {
    ins.op = OR;
    break;
  }
  case 0x2: {
    ins.op = XOR;
    break;
  }

  // One outlyer case
  case 0x3:
    ins.op = NOT;
    ins.kinds[0] = ins.kinds[1];
    ins.kinds[1] = ArgKind::NONE;
    if (op_nib_low > 3) {
      // Not encoded
      ins.op = NONE;
      break;
    }
    break;
  case 0x8:
    // ROL | ROR
    if (op_nib_low > 7) {
      // Not encoded
      ins.op = NONE;
      break;
    }
    ins.op = (op_nib_low > 3) ? ROR : ROL;
    ins.kinds[0] = ins.kinds[1];
    ins.kinds[1] = ArgKind::NONE;
    break;
  case 0xD: {
    if (op_nib_up == 0xD)
      ins.op = EXT;
    // In these cases single arg DN is stored in final nib.
    ins.kinds[0] = ins.kinds[1];
    ins.kinds[1] = ArgKind::NONE;
    break;
  }

  case 0x4: {
    ins.op = MUL;
    break;
  }
  case 0x5: {
    ins.op = MULU;
    break;
  }
  case 0x6: {
    ins.op = DIV;
    break;
  }
  case 0x7: {
    ins.op = DIVU;
    break;
  }
  case 0x9:
    ins.op = ASL;
  case 0xA:
  case 0xB: {
    if (op_nib_up == 0xb)
      ins.op = ASR;
    if (op_nib_up == 0xa)
      ins.op = LSR;

    break;
  }
  case 0xC: {
    ins.op = NONE;
    break;
  }
  case 0xE: {
    if (op_nib_low > 7) {
      // Not encoded
      ins.op = NONE;
      break;
    }
    ins.op = MOV;
    ins.kinds[0] = (op_nib_low > 3) ? ArgKind::PSW : ArgKind::MDR;
    break;
  }
  case 0xF: {
    ins.op = MOV;
    idx = op_nib_low % 4;
    if (idx == 1) {
      // No instruction here
      ins.op = NONE;
      break;
    }
    // This ones a bit strange
    const ArgKind kinds[] = {ArgKind::SP, ArgKind::MDR, ArgKind::PSW};
    // This or i add an element to pad out the array and i cba
    if (idx > 1)
      idx--;
    ins.kinds[0] = ins.kinds[1];
    ins.kinds[1] = kinds[idx];
    use_d = idx >= 1 ? dn_idx : an_idx;
    idx = reg_low;
    if (!use_d)
      idx = reg_high;
    reg = regs[use_d][idx];
    ins.kinds[0] = reg;
    break;
  }
  }
}

void Decoder::decode_dn_op_F3(const inst_data *data, Instruction &ins) {
  ++data;
  const inst_op op = *data;
  const inst_op op_nib_up = nib_up(op);
  const inst_op op_nib_low = nib_low(op);
  ArgKind reg;
  bool use_d;
  uint8_t idx;

  ins.op = MOV;
  ins.sz = InsSzDn::D0s;

  ArgKind a_mem_reg = regs[an_idx][op_nib_low & 0b0011];
  ArgKind di_mem_reg = regs[dn_idx][(op_nib_low & 0b1100) >> 2];

  idx = op_nib_up % 8;
  use_d = op_nib_up < 8;
  reg = regs[use_d][op_nib_up & 0b0011];

  // TODO: How are we making sure we know this is memory?
  // I think due to the num of args/kinds we can infer.
  if (idx > 3) {
    ins.kinds[0] = reg;
    ins.kinds[1] = di_mem_reg;
    ins.kinds[2] = a_mem_reg;
  } else {
    ins.kinds[0] = di_mem_reg;
    ins.kinds[1] = a_mem_reg;
    ins.kinds[2] = reg;
  }
}

void Decoder::decode_dn_op_F4(const inst_data *data, Instruction &ins) {
  ++data;
  const inst_op op = *data;
  const inst_op op_nib_up = nib_up(op);
  const inst_op op_nib_low = nib_low(op);
  ArgKind reg;
  bool use_d;
  uint8_t idx;

  ins.op = (op_nib_up > 7) ? MOVHU : MOVBU;
  ins.sz = InsSzDn::D0s;

  ArgKind a_mem_reg = regs[an_idx][op_nib_low & 0b0011];
  ArgKind di_mem_reg = regs[dn_idx][(op_nib_low & 0b1100) >> 2];
  idx = op_nib_up % 8;
  reg = regs[dn_idx][op_nib_up & 0b0011];

  if (idx > 3) {
    ins.kinds[0] = reg;
    ins.kinds[1] = di_mem_reg;
    ins.kinds[2] = a_mem_reg;
  } else {
    ins.kinds[0] = di_mem_reg;
    ins.kinds[1] = a_mem_reg;
    ins.kinds[2] = reg;
  }
}

void Decoder::decode_dn_op_F5(const inst_data *data, Instruction &ins) {
  ++data;
  const inst_op op = *data;
  const inst_op op_nib_up = nib_up(op);
  const inst_op op_nib_low = nib_low(op);
  ArgKind reg;
  bool use_d;
  uint8_t idx;

  // All UDF20-UDF35

  /*switch (op_nib_up) {
  case 0x0: {
    break;
  }
  case 0x1: {
    break;
  }
  case 0x2: {
    break;
  }
  case 0x3: {
    break;
  }
  case 0x4: {
    break;
  }
  case 0x5: {
    break;
  }
  case 0x6: {
    break;
  }
  case 0x7: {
    break;
  }
  case 0x8: {
    break;
  }
  case 0x9: {
    break;
  }
  case 0xA: {
    break;
  }
  case 0xB: {
    break;
  }
  case 0xC: {
    break;
  }
  case 0xD: {
    break;
  }
  case 0xE: {
    break;
  }
  case 0xF: {
    break;
  }
  }*/
}

void Decoder::decode_dn_op_F6(const inst_data *data, Instruction &ins) {
  ++data;
  const inst_op op = *data;
  const inst_op op_nib_up = nib_up(op);
  const inst_op op_nib_low = nib_low(op);
  ArgKind reg;
  bool use_d;
  uint8_t idx;
  // All UDF00-UDF15

  /*switch (op_nib_up) {
  case 0x0: {
    break;
  }
  case 0x1: {
    break;
  }
  case 0x2: {
    break;
  }
  case 0x3: {
    break;
  }
  case 0x4: {
    break;
  }
  case 0x5: {
    break;
  }
  case 0x6: {
    break;
  }
  case 0x7: {
    break;
  }
  case 0x8: {
    break;
  }
  case 0x9: {
    break;
  }
  case 0xA: {
    break;
  }
  case 0xB: {
    break;
  }
  case 0xC: {
    break;
  }
  case 0xD: {
    break;
  }
  case 0xE: {
    break;
  }
  case 0xF: {
    break;
  }
  }*/
}

void Decoder::decode_dn_op_F7(const inst_data *data, Instruction &ins) {
  ++data;
  const inst_op op = *data;
  const inst_op op_nib_up = nib_up(op);
  const inst_op op_nib_low = nib_low(op);
  ArgKind reg;
  bool use_d;
  uint8_t idx;
  switch (op_nib_up) {
  case 0x0: {
    break;
  }
  case 0x1: {
    break;
  }
  case 0x2: {
    break;
  }
  case 0x3: {
    break;
  }
  case 0x4: {
    break;
  }
  case 0x5: {
    break;
  }
  case 0x6: {
    break;
  }
  case 0x7: {
    break;
  }
  case 0x8: {
    break;
  }
  case 0x9: {
    break;
  }
  case 0xA: {
    break;
  }
  case 0xB: {
    break;
  }
  case 0xC: {
    break;
  }
  case 0xD: {
    break;
  }
  case 0xE: {
    break;
  }
  case 0xF: {
    break;
  }
  }
}

void Decoder::decode_dn_op_F8(const inst_data *data, Instruction &ins) {
  // TODO: Test
  ++data;
  const inst_op op = *data;
  const inst_op op_nib_up = nib_up(op);
  const inst_op op_nib_low = nib_low(op);
  ArgKind reg;
  bool use_d;
  uint8_t idx;

  this->add_args = true;
  this->arg_sz = 1;

  switch (op_nib_up) {
  case 0x0:
  case 0x1:
  case 0x2:
  case 0x3:
    ins.op = MOV;
  case 0x4:
  case 0x5:
    if (op_nib_up > 0x3)
      ins.op = MOVBU;
  case 0x6:
  case 0x7: {
    // Needed because of the fallthrough
    if (op_nib_up == 0x6 || op_nib_up == 0x7)
      ins.op = MOVHU;

    ins.sz = InsSzDn::D1s;

    ArgKind a_mem_reg = regs[an_idx][op_nib_low & 0b0011];
    ArgKind a_or_d_reg;
    use_d = (op_nib_low < 2 || op_nib_low > 3);
    a_or_d_reg = regs[use_d][(op_nib_low & 0b1100) >> 2];

    idx = op_nib_up % 2;
    if (idx) {
      ins.kinds[0] = a_or_d_reg;
      ins.kinds[1] = ArgKind::d8;
      ins.kinds[2] = a_mem_reg;
    } else {
      ins.kinds[0] = ArgKind::d8;
      ins.kinds[1] = a_mem_reg;
      ins.kinds[2] = a_or_d_reg;
    }

    break;
  }
  case 0x8: {
    ins.op = NONE;
    ins.sz = InsSzDn::D0s;
    break;
  }
  case 0x9: {

    idx = op_nib_low % 4;
    if (idx < 2) {
      ins.op = NONE;
      ins.sz = InsSzDn::D0s;
      break;
    }

    ins.op = (idx == 2) ? MOVBU : MOVHU;
    ins.sz = InsSzDn::D1s;
    ins.kinds[0] = regs[dn_idx][(op_nib_low & 0b1100) >> 2];
    ins.kinds[1] = ArgKind::d8;
    ins.kinds[1] = ArgKind::SP;
    add_args = true;
    arg_sz = 1;

    break;
  }
  case 0xA: {
    ins.op = NONE;
    ins.sz = InsSzDn::D0s;
    break;
  }
  case 0xB: {
    if (op_nib_low < 8) {
      ins.op = NONE;
      ins.sz = InsSzDn::D0s;
      break;
    }

    ins.op = (idx > 0xb) ? MOVHU : MOVBU;
    ins.kinds[0] = ArgKind::d8;
    ins.kinds[1] = ArgKind::SP;
    ins.kinds[2] = regs[dn_idx][op_nib_low & 0b0011];

    add_args = true;
    arg_sz = 1;
    break;
  }
  case 0xC: {
    if (op_nib_low > 0xb) {
      ins.op = NONE;
      ins.sz = InsSzDn::D0s;
      break;
    }

    const InsnType types[] = {ASL, LSR, ASR};
    idx = op_nib_low % 3;
    ins.op = types[idx];
    ins.kinds[0] = ArgKind::imm8;
    ins.kinds[1] = regs[dn_idx][op_nib_low & 0b0011];

    add_args = true;
    arg_sz = 1;

    break;
  }
  case 0xD: {
    ins.op = NONE;
    ins.sz = InsSzDn::D0s;
    break;
  }
  case 0xE: {
    // Annoying one
    idx = op_nib_low >> 2;
    ins.sz = InsSzDn::D1s;
    add_args = true;
    arg_sz = 1;
    if (idx == 2) {
      ins.op = InsnType::BVC + (op_nib_low % 4);
      ins.kinds[0] = ArgKind::d8;
      ins.kinds[1] = ArgKind::PC;
      break;
    }

    ins.op = (idx == 0) ? AND : (idx < 3) ? OR : BTST;
    ins.kinds[0] = ArgKind::imm8;
    ins.kinds[1] = regs[dn_idx][op_nib_low & 0b0011];
    break;
  }
  case 0xF: {
    idx = op_nib_low >> 2;
    add_args = true;
    arg_sz = 1;
    if (idx > 2) {
      if (op_nib_low != 0xE) {
        ins.op = NONE;
        ins.sz = InsSzDn::D0s;
        break;
      }
      ins.op = ADD;
      ins.kinds[0] = ArgKind::imm8;
      ins.kinds[1] = ArgKind::SP;
      ins.sz = InsSzDn::D1s;
      break;
    }

    ins.op = MOV;
    reg = regs[an_idx][op_nib_low & 0b0011];
    if (idx < 1) {
      ins.kinds[0] = ArgKind::d8;
      ins.kinds[1] = reg;
      ins.kinds[2] = ArgKind::SP;
    } else {
      ins.kinds[0] = ArgKind::SP;
      ins.kinds[1] = ArgKind::d8;
      ins.kinds[2] = reg;
    }
    ins.sz = InsSzDn::D1s;

    break;
  }
  }
}

void Decoder::decode_dn_op_F9(const inst_data *data, Instruction &ins) {
  // TODO: Test
  ++data;
  const inst_op op = *data;
  const inst_op op_nib_up = nib_up(op);
  const inst_op op_nib_low = nib_low(op);
  ArgKind reg;
  bool use_d;
  uint8_t idx;

  // All UDF00-UDF35

  /*switch (op_nib_up) {
  case 0x0: {
    break;
  }
  case 0x1: {
    break;
  }
  case 0x2: {
    break;
  }
  case 0x3: {
    break;
  }
  case 0x4: {
    break;
  }
  case 0x5: {
    break;
  }
  case 0x6: {
    break;
  }
  case 0x7: {
    break;
  }
  case 0x8: {
    break;
  }
  case 0x9: {
    break;
  }
  case 0xA: {
    break;
  }
  case 0xB: {
    break;
  }
  case 0xC: {
    break;
  }
  case 0xD: {
    break;
  }
  case 0xE: {
    break;
  }
  case 0xF: {
    break;
  }
  }*/
}

// Marked as 0xF4 in the manual, silly
void Decoder::decode_dn_op_FA(const inst_data *data, Instruction &ins) {
  ++data;
  const inst_op op = *data;
  const inst_op op_nib_up = nib_up(op);
  const inst_op op_nib_low = nib_low(op);
  ArgKind reg;
  bool use_d;
  uint8_t idx;

  // Most of this is 16bit
  add_args = true;
  arg_sz = 2;

  switch (op_nib_up) {
  case 0x0:
  case 0x1:
  case 0x2:
  case 0x3:
    ins.op = MOV;
  case 0x4:
  case 0x5:
    if (op_nib_up >= 0x4)
      ins.op = MOVBU;
  case 0x6:
  case 0x7: {
    // Needed because of the fallthrough
    if (op_nib_up >= 0x6)
      ins.op = MOVHU;

    ins.sz = InsSzDn::D2s;

    ArgKind a_mem_reg = regs[an_idx][op_nib_low & 0b0011];
    ArgKind a_or_d_reg;
    use_d = !(op_nib_up == 2 || op_nib_up == 3);
    a_or_d_reg = regs[use_d][(op_nib_low & 0b1100) >> 2];

    idx = op_nib_low % 2;
    if (!idx) {
      ins.kinds[0] = a_or_d_reg;
      ins.kinds[1] = ArgKind::d16;
      ins.kinds[2] = a_mem_reg;
    } else {
      ins.kinds[0] = ArgKind::d16;
      ins.kinds[1] = a_mem_reg;
      ins.kinds[2] = a_or_d_reg;
    }

    break;
  }

  case 0x8:
  case 0x9: {
    idx = op_nib_low % 4;
    if (op_nib_up == 0x8 && idx > 0) {
      ins.op = NONE;
      ins.sz = InsSzDn::D0s;
      break;
    }

    add_args = true;
    arg_sz = 2;

    ins.op = (idx < 2) ? MOV : (idx < 3) ? MOVBU : MOVHU;
    ins.sz = InsSzDn::D2s;
    use_d = (idx != 0);
    ins.kinds[0] = regs[use_d][(op_nib_low & 0b1100) >> 2];
    if (op_nib_up == 0x8) {
      ins.kinds[1] = ArgKind::abs16;
    } else {
      ins.kinds[1] = ArgKind::d16;
      ins.kinds[2] = ArgKind::SP;
    }
    break;
  }
  case 0xA:
  case 0xB: {
    if (op_nib_up == 0xA && op_nib_low > 3) {
      ins.op = NONE;
      ins.sz = InsSzDn::D0s;
      break;
    }

    add_args = true;
    arg_sz = 2;

    ins.op = (op_nib_low < 8) ? MOV : (op_nib_low < 0xC) ? MOVBU : MOVHU;
    ins.sz = InsSzDn::D2s;
    use_d = (op_nib_low > 3);
    reg = regs[use_d][op_nib_low & 0b0011];
    if (op_nib_up == 0xA) {
      ins.kinds[0] = ArgKind::abs16;
      ins.kinds[1] = reg;
    } else {
      ins.kinds[0] = ArgKind::d16;
      ins.kinds[1] = ArgKind::SP;
      ins.kinds[2] = reg;
    }

    break;
  }
  case 0xC:
  case 0xD: {
    idx = op_nib_low % 8;
    if (idx > 3) {
      ins.op = NONE;
      ins.sz = InsSzDn::D0s;
      break;
    }
    ins.op = (op_nib_low < 4) ? ADD : CMP;
  }
  case 0xE: {

    add_args = true;
    arg_sz = 2;
    if (op_nib_up == 0xE)
      ins.op = (op_nib_low < 4)     ? AND
               : (op_nib_low < 8)   ? OR
               : (op_nib_low < 0xC) ? XOR
                                    : BTST;

    use_d = op_nib_up != 0xD;
    reg = regs[use_d][op_nib_low & 0b0011];

    ins.kinds[0] = ArgKind::imm16;
    ins.kinds[1] = reg;
    break;
  }
  case 0xF: {

    ins.sz = InsSzDn::D2s;

    add_args = true;

    if (op_nib_low < 0xC) {
      num_args = 2;
      arg_sz = 2;
      idx = op_nib_low >> 2;
      ins.op = InsnType::BSET + idx;
      // D8 comes first in memory.
      ins.kinds[0] = ArgKind::imm8;
      ins.kinds[1] = ArgKind::d8;
      ins.kinds[2] = regs[an_idx][op_nib_low & 0b0011];
    } else {
      arg_sz = 2;
      idx = op_nib_low % 4;
      ins.op = (idx < 1) ? AND : (idx < 2) ? OR : (idx < 3) ? ADD : CALLS;
      ins.kinds[0] = (idx < 3) ? ArgKind::imm16 : ArgKind::d16;
      ins.kinds[1] = (idx < 2)   ? ArgKind::PSW
                     : (idx < 3) ? ArgKind::SP
                                 : ArgKind::PC;
    }
    break;
  }
  }
}

void Decoder::decode_dn_op_FB(const inst_data *data, Instruction &ins) {
  // TODO: Test
  ++data;
  const inst_op op = *data;
  const inst_op op_nib_up = nib_up(op);
  const inst_op op_nib_low = nib_low(op);
  ArgKind reg;
  bool use_d;
  uint8_t idx;
  ins.op = NONE;
  ins.sz = InsSzDn::D0s;
  switch (op_nib_up) {
  case 0x0: {
    break;
  }
  case 0x1: {
    break;
  }
  case 0x2: {
    break;
  }
  case 0x3: {
    break;
  }
  case 0x4: {
    break;
  }
  case 0x5: {
    break;
  }
  case 0x6: {
    break;
  }
  case 0x7: {
    break;
  }
  case 0x8: {
    break;
  }
  case 0x9: {
    break;
  }
  case 0xA: {
    break;
  }
  case 0xB: {
    break;
  }
  case 0xC: {
    break;
  }
  case 0xD: {
    break;
  }
  case 0xE: {
    break;
  }
  case 0xF: {
    break;
  }
  }
}

void Decoder::decode_dn_op_FC(const inst_data *data, Instruction &ins) {
  ++data;
  const inst_op op = *data;
  const inst_op op_nib_up = nib_up(op);
  const inst_op op_nib_low = nib_low(op);
  ArgKind reg;
  bool use_d;
  uint8_t idx;
  switch (op_nib_up) {
  case 0x0:
  case 0x1:
  case 0x2:
  case 0x3:
    ins.op = MOV;
  case 0x4:
  case 0x5:
    if (op_nib_up > 0x3)
      ins.op = MOVBU;
  case 0x6:
  case 0x7: {
    if (op_nib_up == 0x6 || op_nib_up == 0x7) {
      ins.op = MOVHU;
    }

    add_args = true;
    arg_sz = 4;

    ins.sz = InsSzDn::D4s;
    idx = op_nib_up % 2;

    use_d = !(op_nib_up == 2 || op_nib_up == 3);

    const ArgKind reg = regs[use_d][(op_nib_low & 0b1100) >> 2];
    const ArgKind an_reg = regs[an_idx][op_nib_low & 0b0011];
    if (!idx) {
      ins.kinds[0] = ArgKind::d32;
      ins.kinds[1] = an_reg;
      ins.kinds[2] = reg;
    } else {
      ins.kinds[0] = reg;
      ins.kinds[1] = ArgKind::d32;
      ins.kinds[2] = an_reg;
    }
    break;
  }
  case 0x8:
  case 0x9: {
    // TODO: Test
    ins.sz = InsSzDn::D4s;
    idx = op_nib_low % 4;
    ins.op = (idx < 2) ? MOV : (idx < 3) ? MOVBU : MOVHU;
    use_d = (idx > 0);
    ins.kinds[0] = regs[use_d][(op_nib_low & 0b1100) >> 2];

    if (op_nib_up == 8)
      ins.kinds[1] = ArgKind::abs32;
    else {
      ins.kinds[1] = ArgKind::d32;
      ins.kinds[2] = ArgKind::SP;
    }
    add_args = true;
    arg_sz = 4;
    break;
  }
  case 0xA:
  case 0xB: {
    ins.sz = InsSzDn::D4s;
    idx = op_nib_low >> 2;
    ins.op = (idx < 2) ? MOV : (idx < 3) ? MOVBU : MOVHU;
    use_d = (idx > 0);

    reg = regs[use_d][op_nib_low & 0b0011];
    if (op_nib_up == 0xA) {
      ins.kinds[0] = ArgKind::abs32;
      ins.kinds[1] = reg;
    } else {
      ins.kinds[0] = ArgKind::d32;
      ins.kinds[1] = ArgKind::SP;
      ins.kinds[2] = reg;
    }

    add_args = true;
    arg_sz = 4;
    break;
  }
  case 0xC:
  case 0xD:
  case 0xE: {

    ins.sz = InsSzDn::D4s;
    ins.kinds[0] = ArgKind::imm32;

    if (op_nib_up < 0xE)
      ins.op = (op_nib_low > 0xB)   ? MOV
               : (op_nib_low > 0x7) ? CMP
               : (op_nib_low > 0x3) ? SUB
                                    : ADD;
    else
      ins.op = (op_nib_low > 0xB)   ? BTST
               : (op_nib_low > 0x7) ? XOR
               : (op_nib_low > 0x3) ? OR
                                    : AND;

    use_d = (op_nib_up != 0xD);
    ins.kinds[1] = regs[use_d][op_nib_low & 0b0011];
    add_args = true;
    arg_sz = 4;

    break;
  }
  case 0xF: {

    add_args = true;
    arg_sz = 4;
    if (op_nib_low < 0xE) {
      ins.op = NONE;
      ins.sz = InsSzDn::D0s;
    }
    ins.sz = InsSzDn::D4s;
    ins.op = (op_nib_low == 0xE) ? ADD : CALLS;
    if (ins.op == ADD) {
      ins.kinds[0] = ArgKind::imm32;
      ins.kinds[1] = ArgKind::SP;
    } else {
      ins.kinds[0] = ArgKind::d32;
      ins.kinds[1] = ArgKind::PC;
    }
    break;
  }
  }
}

void Decoder::decode_dn_op_FD(const inst_data *data, Instruction &ins) {
  // TODO: Test
  ++data;
  const inst_op op = *data;
  const inst_op op_nib_up = nib_up(op);
  const inst_op op_nib_low = nib_low(op);
  ArgKind reg;
  bool use_d;
  uint8_t idx;
  ins.op = NONE;
  ins.sz = InsSzDn::D0s;
  switch (op_nib_up) {
  case 0x0: {
    break;
  }
  case 0x1: {
    break;
  }
  case 0x2: {
    break;
  }
  case 0x3: {
    break;
  }
  case 0x4: {
    break;
  }
  case 0x5: {
    break;
  }
  case 0x6: {
    break;
  }
  case 0x7: {
    break;
  }
  case 0x8: {
    break;
  }
  case 0x9: {
    break;
  }
  case 0xA: {
    break;
  }
  case 0xB: {
    break;
  }
  case 0xC: {
    break;
  }
  case 0xD: {
    break;
  }
  case 0xE: {
    break;
  }
  case 0xF: {
    break;
  }
  }
}

// This is gonna require some tinkering.
void Decoder::decode_dn_op_FE(const inst_data *data, Instruction &ins) {
  // TODO: Test
  ++data;
  const inst_op op = *data;
  const inst_op op_nib_up = nib_up(op);
  const inst_op op_nib_low = nib_low(op);
  ArgKind reg;
  bool use_d;
  uint8_t idx;
  switch (op_nib_up) {
  case 0x0:
  case 0x8: {

    if (op_nib_low > 2) {
      ins.op = NONE;
      ins.sz = InsSzDn::D0s;
      break;
    }

    add_args = true;
    num_args = 2;
    ins.op = InsnType::BSET + op_nib_low;
    ins.kinds[0] = ArgKind::imm8;

    if (op_nib_up == 0) {
      ins.kinds[1] = ArgKind::abs32;
      ins.sz = InsSzDn::D5s;
      arg_sz = 5;
    } else {
      ins.kinds[1] = ArgKind::abs16;
      ins.sz = InsSzDn::D3s;
      arg_sz = 3;
    }

    break;
  }
  default: {
    ins.op = NONE;
    ins.sz = InsSzDn::D0s;
    break;
  }
  }
}

void Decoder::decode_dn_op_FF(const inst_data *data, Instruction &ins) {
  // TODO: Test, also is this needed?

  ++data;
  const inst_op op = *data;
  const inst_op op_nib_up = nib_up(op);
  const inst_op op_nib_low = nib_low(op);
  ArgKind reg;
  bool use_d;
  uint8_t idx;
  switch (op_nib_up) {
  case 0x0: {
    break;
  }
  case 0x1: {
    break;
  }
  case 0x2: {
    break;
  }
  case 0x3: {
    break;
  }
  case 0x4: {
    break;
  }
  case 0x5: {
    break;
  }
  case 0x6: {
    break;
  }
  case 0x7: {
    break;
  }
  case 0x8: {
    break;
  }
  case 0x9: {
    break;
  }
  case 0xA: {
    break;
  }
  case 0xB: {
    break;
  }
  case 0xC: {
    break;
  }
  case 0xD: {
    break;
  }
  case 0xE: {
    break;
  }
  case 0xF: {
    break;
  }
  }
}
// Lotta possibilities here.
void Decoder::decode_dn_op(const inst_data *data, Instruction &ins_out) {
  inst_op op = *data;
  this->add_args = false;
  this->arg_sz = 0;
  this->num_args = 1;
  // TODO: use this

  // This is going to be absolutely monsterous.
  switch (op) {
  case 0xF0:
    decode_dn_op_F0(data, ins_out);
    break;
  case 0xF1:
    decode_dn_op_F1(data, ins_out);
    break;
  case 0xF2:
    decode_dn_op_F2(data, ins_out);
    break;
  case 0xF3:
    decode_dn_op_F3(data, ins_out);
    break;
  case 0xF4:
    decode_dn_op_F4(data, ins_out);
    break;
  case 0xF5:
    decode_dn_op_F5(data, ins_out);
    break;
  case 0xF6:
    decode_dn_op_F6(data, ins_out);
    break;
  case 0xF7:
    // decode_dn_op_F7(data, ins_out);
    //  Reserved
    break;
  case 0xF8:
    decode_dn_op_F8(data, ins_out);
    break;
  case 0xF9:
    decode_dn_op_F9(data, ins_out);
    break;
  case 0xFA:
    decode_dn_op_FA(data, ins_out);
    break;
  case 0xFB:
    decode_dn_op_FB(data, ins_out);
    break;
  case 0xFC:
    decode_dn_op_FC(data, ins_out);
    break;
  case 0xFD:
    decode_dn_op_FD(data, ins_out);
    break;
  case 0xFE:
    decode_dn_op_FE(data, ins_out);
    break;
  case 0xFF:
    std::cerr << "Decoder::decode_dn_op tried calling 0xFF handler, WTF?"
              << std::endl;
    abort();
    // decode_dn_op_FF(data, ins_out);
    break;
  }

  if (add_args && arg_sz) {
    // Extra +1 cuz data is pointing at the first op byte atm
    data += 2;
    if (data + arg_sz > this->end) {
      // Cant decode if we go oob
      std::cout << "Decoder::decode_dn_op truncating args" << std::endl;
      ins_out.sz = 0;
      arg_sz = 0;
    }
    /// TODOOO
    if (num_args > 1) {
      ins_out.copy_data_args(data, arg_sz);
    } else {
      ins_out.arg_add(data, arg_sz);
    }
    // ins_out.arg_add();
  }

  ins_out.log();
}

void Decoder::decode_inst(const inst_data *curr_data, const inst_data *end,
                          Instruction &ins) {

  // Save end ptr so we can check against it in decoding.
  this->end = end;
  // TODO: Actually decode stuff
  ins.op = InsnType::NOP;
  ins.sz = 0;
  ins.curr = 0;
  // Dont forget to
  memset(&ins.args, 0, sizeof(ins.args));
  memset(&ins.kinds, 0, sizeof(ins.kinds));

  if (curr_data >= end)
    return;

  inst_data opcode = *curr_data;
  // std::cout << "Decoder::decode_inst opcode data: 0x" << std::hex <<
  // (int)opcode
  //           << std::endl;

  // Go over possible opcodes to determine size
  switch (opcode) {
  case 0xF5:
  case 0xF6:
    // F5 and F6 is exclusively UDF
    ins.op = InsnType::UDFnn;
  case 0xF1:
  case 0xF2:
  case 0xF3:
  case 0xF4:
    ins.sz = 4;
    break;
  case 0xF8:
  case 0xF9:
    ins.sz = 3;
    break;
  case 0xFA:
  case 0xFB:
    ins.sz = 4;
    break;
  case 0xFC:
  case 0xFD:
    ins.sz = 6;
    break;
  case 0xFE:
    // Could be 5 or 7, will try to determine that for sure later.
    ins.sz = 5;
    break;
  default:
    // This handles unexpected opcodes, 0xF7, as well as Sn opcodes, so we will
    // assume S0 and 1 byte size until we get more info.
    std::cout << "Decoder::decode_inst opcode: " << (int)opcode << std::endl;
    ins.op = InsnType::NONE;
    ins.sz = 1;
    break;
  }

  // Cant decode reserved instructions.
  if (ins.op == InsnType::NONE && opcode == 0xF7) {
    ins.sz = 0;
    return;
  }

  if (opcode < 0xF0) {
    this->decode_sn_op(curr_data, ins);
  } else {
    this->decode_dn_op(curr_data, ins);
  }

  return;
}
