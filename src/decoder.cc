#include "decoder.h"
#include "instruction.h"
#include "opcode.h"
#include "registers.h"
#include <cstring>
#include <iostream>
#include <ostream>

static inline inst_data nib_up(inst_op op) { return (op & 0xf0) >> 4; }
static inline inst_data nib_low(inst_op op) { return (op & 0xf); }
// Handler function for Sn ops
static void handle_sn(inst_data *data, Instruction &ins_out) {}

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
  bool add_args = false;
  uint8_t arg_sz = 0;
  // TODO: use this
  const uint8_t dn_idx = 1;
  const uint8_t an_idx = 0;
  const ArgKind *regs[] = {an_registers, dn_registers};

  // Old stuff.
  //  if (op_nib_up < 1) {
  //    std::cout << "Decoder::decode_sn_op entering op_nib_up < 1 branch"
  //              << std::endl;
  //    // For Sn, this will either be a CLR or a mov variation with the Dn
  //    // registers. Lets eliminate the former first.
  //    const inst_op mask_clr = 0b1100;
  //    reg = dn_registers[(op_nib_low & mask_clr) >> 2];
  //    ins_out.kinds[0] = reg;
  //    if ((mask_clr ^ op_nib_low) == 0) {
  //      // Issa CLR. We know it'll be on one of the Dn registers
  //      // Then need to store it on the instruction.
  //      std::cout << "Decoder::decode_sn_op instruction is CLR" << std::endl;
  //      ins_out.op = InsnType::CLR;
  //      ins_out.sz = InsSzSn::S0;
  //      return;
  //    }
  //
  //    // mov/hu/bu Dn,(abs16)
  //    const inst_data movhu_sn_upper = 0xf;
  //    const inst_data movbu_sn_upper = 0xe;
  //    const inst_data mov_sn_upper = 0xd;
  //
  //    ins_out.kinds[1] = ArgKind::abs16;
  //    ins_out.sz = InsSzSn::S2;
  //
  //    if ((movhu_sn_upper - op) % 0x4 == 0) {
  //      // Probably a movhu inst
  //      std::cout << "Decoder::decode_sn_op instruction is MOVHU" <<
  //      std::endl; ins_out.op = InsnType::MOVHU;
  //    } else if ((movbu_sn_upper - op) % 0x4 == 0) {
  //      // Probably a movbu
  //      std::cout << "Decoder::decode_sn_op instruction is MOVBU" <<
  //      std::endl; ins_out.op = InsnType::MOVBU;
  //    } else {
  //      // Gotta be a plain mov
  //      std::cout << "Decoder::decode_sn_op instruction is MOV" << std::endl;
  //      ins_out.op = InsnType::MOV;
  //    }
  //    // All the above have abs16 for second.
  //    // Need +1 to skip past the opcode
  //    add_args = true;
  //    arg_sz = 2;
  //  } else if (op_nib_up < 2) {
  //    // TODO: BIG: Test decoding these instructions branches below.
  //    const InsnType types[] = {EXTB, EXTBU, EXTH, EXTHU};
  //    // Handles EXTB/BU/H/HU variants
  //    ins_out.sz = InsSzSn::S0;
  //    // Didnt feel like sketching out a whole ass if else chain.
  //    // Will be max 3
  //    ins_out.op = types[op_nib_low >> 2];
  //    ArgKind reg = dn_registers[op_nib_low & 0b0011];
  //    ins_out.kinds[0] = reg;
  //  } else if (op_nib_up < 3) {
  //    // Mov or add variation with a imm8 or imm16
  //    const InsnType types[] = {ADD, MOV};
  //    ins_out.op = types[op_nib_low & 1];
  //    std::cout << "Decoder::decode_sn_op instruction is ";
  //    if (ins_out.op == MOV) {
  //      std::cout << "\"MOV imm16, ";
  //      ins_out.kinds[0] = ArgKind::imm16;
  //      ins_out.sz = InsSzSn::S2;
  //    } else {
  //      std::cout << "\"ADD imm8, ";
  //      ins_out.kinds[0] = ArgKind::imm8;
  //      ins_out.sz = InsSzSn::S1;
  //    }
  //
  //    use_d = op_nib_low & 0b10;
  //    idx = (op_nib_low & 0b1100) >> 2;
  //    if (use_d) {
  //      std::cout << "d" << (int)idx << "\"" << std::endl;
  //      reg = dn_registers[idx];
  //    } else {
  //      std::cout << "a" << (int)idx << std::endl;
  //      reg = an_registers[idx];
  //    }
  //    ins_out.kinds[1] = reg;
  //    add_args = true;
  //    arg_sz = (op_nib_low & 1) + 1;
  //  } else if (op_nib_up < 4) {
  //    // TODO: Test
  //    // Mov/b/hu variation with either SP or abs16
  //    const InsnType types[] = {MOV, MOVBU, MOVHU, MOV};
  //    idx = op_nib_low & 0b11;
  //    ins_out.op = types[idx];
  //    if (idx < 3) {
  //      reg = dn_registers[idx];
  //      ins_out.kinds[0] = ArgKind::abs16;
  //      add_args = true;
  //      arg_sz = 2;
  //      ins_out.sz = InsSzSn::S3;
  //    } else {
  //      reg = an_registers[idx];
  //      ins_out.kinds[0] = ArgKind::SP;
  //      ins_out.sz = InsSzSn::S0;
  //    }
  //    ins_out.kinds[1] = reg;
  //  } else if (op_nib_up < 5) {
  //    // TODO: Test
  //    // Inc or mov variation
  //    const InsnType types[] = {INC, INC, MOV, MOV};
  //    idx = op_nib_low & 0b11;
  //
  //    ins_out.op = types[idx];
  //    use_d = !(idx & 0b01);
  //    ins_out.kinds[0] = regs[use_d][idx];
  //    if (idx & 0b10) {
  //      // Need to set kinds[1] and set size
  //      // MOV (D|A)n,(d8,SP)
  //      ins_out.kinds[1] = ArgKind::d8;
  //      ins_out.kinds[2] = ArgKind::SP;
  //      ins_out.sz = InsSzSn::S1;
  //    } else {
  //      ins_out.sz = InsSzSn::S0;
  //    }
  //  } else if (op_nib_up < 0x6) {
  //    // TODO: Test
  //    // INC, ASL, or mov variation
  //    const InsnType types[] = {INC4, ASL2, MOV, MOV};
  //    idx = op_nib_low >> 2;
  //    ins_out.op = types[idx];
  //
  //    use_d = (op_nib_low % 0x8) < 4;
  //    if (op_nib_low > 0x7) {
  //      ins_out.kinds[0] = regs[use_d][op_nib_low % 4];
  //      ins_out.kinds[1] = ArgKind::d8;
  //      ins_out.kinds[2] = ArgKind::SP;
  //      ins_out.sz = InsSzSn::S1;
  //      add_args = true;
  //      arg_sz = 1;
  //    } else {
  //      ins_out.kinds[0] = regs[(!use_d)][op_nib_low % 4];
  //      ins_out.sz = InsSzSn::S0;
  //    }
  //  } else if (op_nib_up < 0x7) {
  //    // TODO: Test
  //    // TODO: Need some way to
  //    // This is all mov dm, (an)
  //    ins_out.op = MOV;
  //    ins_out.sz = InsSzSn::S0;
  //    ins_out.kinds[0] = regs[dn_idx][op_nib_low & 0b1100 >> 2];
  //    ins_out.kinds[1] = regs[an_idx][(op_nib_low & 0b0011) + mem_id];
  //  } else if (op_nib_up < 0x8) {
  //    // TODO: Test
  //    // This is all mov (am), dn
  //    ins_out.op = MOV;
  //    ins_out.sz = InsSzSn::S0;
  //    ins_out.kinds[0] = regs[an_idx][(op_nib_low & 0b0011) + mem_id];
  //    ins_out.kinds[1] = regs[dn_idx][op_nib_low & 0b1100 >> 2];
  //  }

  switch (op_nib_up) {
  case 0x0: {
    std::cout << "Decoder::decode_sn_op entering op_nib_up < 1 branch"
              << std::endl;
    // For Sn, this will either be a CLR or a mov variation with the Dn
    // registers. Lets eliminate the former first.
    const inst_op mask_clr = 0b1100;
    reg = dn_registers[(op_nib_low & mask_clr) >> 2];
    ins_out.kinds[0] = reg;
    if ((mask_clr ^ op_nib_low) == 0) {
      // Issa CLR. We know it'll be on one of the Dn registers
      // Then need to store it on the instruction.
      std::cout << "Decoder::decode_sn_op instruction is CLR" << std::endl;
      ins_out.op = InsnType::CLR;
      ins_out.sz = InsSzSn::S0;
      return;
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
    const InsnType types[] = {ADD, MOV};
    ins_out.op = types[op_nib_low & 1];
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
    idx = (op_nib_low & 0b1100) >> 2;
    if (use_d) {
      std::cout << "d" << (int)idx << "\"" << std::endl;
      reg = dn_registers[idx];
    } else {
      std::cout << "a" << (int)idx << std::endl;
      reg = an_registers[idx];
    }
    ins_out.kinds[1] = reg;
    add_args = true;
    // arg_sz = (op_nib_low & 1);
    arg_sz = (op_nib_low & 1) + 1;

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
  case 0x6:
    // TODO: Test
    // TODO: Need some way to
    // This is all mov dm, (an)
    ins_out.op = MOV;
    ins_out.sz = InsSzSn::S0;
    ins_out.kinds[0] = regs[dn_idx][op_nib_low & 0b1100 >> 2];
    ins_out.kinds[1] = regs[an_idx][(op_nib_low & 0b0011) + mem_id];

    break;
  case 0x7: {
    // TODO: Test
    // This is all mov (am), dn
    ins_out.op = MOV;
    ins_out.sz = InsSzSn::S0;
    ins_out.kinds[0] = regs[an_idx][(op_nib_low & 0b0011) + mem_id];
    ins_out.kinds[1] = regs[dn_idx][op_nib_low & 0b1100 >> 2];
    break;
  }
  case 0x8: {
    // TODO: Test
    // Mov Dm, Dn
    ins_out.op = MOV;
    ins_out.kinds[1] = regs[dn_idx][op_nib_low & 0b0011];
    if ((op_nib_low & 0b1100 >> 2) == (op_nib_low & 0b0011)) {
      // Needs to be converted to mov imm8, dn
      ins_out.sz = InsSzSn::S1;
      ins_out.kinds[0] = ArgKind::imm8;
      add_args = true;
      arg_sz = 1;
      break;
    }
    ins_out.sz = InsSzSn::S0;
    ins_out.kinds[0] = regs[dn_idx][op_nib_low & 0b1100 >> 2];
    break;
  }
  case 0x9: {
    // TODO: Test
    // Mov Am, An
    ins_out.op = MOV;
    ins_out.kinds[1] = regs[an_idx][op_nib_low & 0b0011];
    if ((op_nib_low & 0b1100 >> 2) == (op_nib_low & 0b0011)) {
      // Mov imm8, an
      ins_out.kinds[0] = ArgKind::imm8;
      ins_out.sz = InsSzSn::S1;
      add_args = true;
      arg_sz = 1;
      break;
    }
    ins_out.sz = InsSzSn::S0;
    ins_out.kinds[0] = regs[an_idx][op_nib_low & 0b1100 >> 2];
    break;
  }
  case 0xa: {
    // TODO: Test
    // Cmp Dm, Dn
    ins_out.op = CMP;
    ins_out.kinds[1] = regs[dn_idx][op_nib_low & 0b1100 >> 2];
    if ((op_nib_low & 0b1100 >> 2) == (op_nib_low & 0b0011)) {
      // Cmp imm8, Dn
      ins_out.kinds[0] = ArgKind::imm8;
      ins_out.sz = InsSzSn::S1;
      add_args = true;
      arg_sz = 1;
      break;
    }
    ins_out.sz = InsSzSn::S0;
    ins_out.kinds[0] = regs[dn_idx][op_nib_low & 0b1100 >> 2];
    break;
  }
  case 0xb: {
    // TODO: Test
    // Cmp Am, An
    ins_out.op = CMP;
    ins_out.kinds[1] = regs[an_idx][op_nib_low & 0b1100 >> 2];
    if ((op_nib_low & 0b1100 >> 2) == (op_nib_low & 0b0011)) {
      // Cmp imm8, An
      ins_out.kinds[0] = ArgKind::imm8;
      ins_out.sz = InsSzSn::S1;
      add_args = true;
      arg_sz = 1;
      break;
    }
    ins_out.sz = InsSzSn::S0;
    ins_out.kinds[0] = regs[an_idx][op_nib_low & 0b1100 >> 2];
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
    ins_out.kinds[0] = regs[dn_idx][op_nib_low & 0b1100 >> 2];
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
    ins_out.arg_add(data, arg_sz);
  }

  ins_out.log();
}
// Lotta possibilities here.
void Decoder::decode_dn_op(const inst_data *data, Instruction &ins_out) {
  inst_op op = *data;
  inst_op op_nib_up = nib_up(op);
  inst_op op_nib_low = nib_low(op);
  ArgKind reg;
  bool use_d;
  uint8_t idx;
  bool add_args = false;
  uint8_t arg_sz = 0;
  // TODO: use this
  const uint8_t dn_idx = 1;
  const uint8_t an_idx = 0;
  const ArgKind *regs[] = {an_registers, dn_registers};

  if (add_args && arg_sz) {
    data++;
    ins_out.arg_add(data, arg_sz);
  }

  ins_out.log();
}

void Decoder::decode_inst(const inst_data *curr_data, const inst_data *end,
                          Instruction &ins) {

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
  std::cout << "Decoder::decode_inst opcode data: 0x" << std::hex << (int)opcode
            << std::endl;

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
