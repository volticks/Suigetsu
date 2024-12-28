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

  if (op_nib_up < 1) {
    std::cout << "Decoder::decode_sn_op entering op_nib_up < 1 branch"
              << std::endl;
    // For Sn, this will either be a CLR or a mov variation with the Dn
    // registers. Lets eliminate the former first.
    const inst_op mask_clr = 0b1100;
    ArgKind reg = dn_registers[(op_nib_low & mask_clr) >> 2];
    ins_out.kinds[0] = reg;
    if ((mask_clr ^ op_nib_low) == 0) {
      // Issa CLR. We know it'll be on one of the Dn registers
      // Then need to store it on the instruction.
      std::cout << "Decoder::decode_sn_op instruction is CLR" << std::endl;
      ins_out.op = InsnType::CLR;
      ins_out.sz = (int)InsSzSn::S0;
      ins_out.sz = 1;
      return;
    }

    // mov/hu/bu Dn,(abs16)
    const inst_data movhu_sn_upper = 0xf;
    const inst_data movbu_sn_upper = 0xe;
    const inst_data mov_sn_upper = 0xd;

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
    data++;
    ins_out.arg_add(data, 2);
    ins_out.log();
    return;
  }
}
// Lotta possibilities here.
void Decoder::decode_dn_op(const inst_data *data, Instruction &ins_out) {}

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
