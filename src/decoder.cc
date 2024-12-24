#include "decoder.h"
#include "instruction.h"
#include "opcode.h"
#include <cstring>
#include <iostream>
#include <ostream>

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
  inst_data op = *data;
  opcode_type op_cat = Opcodes::op_arr[op & 0xf];
  switch (op_cat) {
  case InsnType::CLR_OR_MOV_S0:
    // Contains set bits which would only be used by a CLR instruction
    //
    ins_out.sz = 1;
    uint32_t mask = 0b00001100;
    if (~mask & op) {
      // This is a MOV variation, do mov stuff here
      ins_out.op = InsnType::NOP;
    }
    // Otherwise we found CLR
    ins_out.op = InsnType::CLR;
    break;
  }
}
// Lotta possibilities here.
void Decoder::decode_dn_op(const inst_data *data, Instruction &ins_out) {}

Instruction Decoder::decode_inst(const inst_data *curr_data,
                                 const inst_data *end) {

  Instruction ins;
  // TODO: Actually decode stuff
  ins.op = InsnType::NOP;
  ins.sz = 0;
  memset(&ins.args, 0, sizeof(ins.args));

  if (curr_data >= end)
    return ins;

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
    std::cerr << "Decoder::decode_inst opcode: " << opcode << std::endl;
    ins.op = InsnType::NONE;
    ins.sz = 1;
    break;
  }

  // Cant decode reserved instructions.
  if (ins.op == InsnType::NONE && opcode == 0xF7) {
    ins.sz = 0;
    return ins;
  }

  if (opcode < 0xF0) {
    this->decode_sn_op(curr_data, ins);
  } else {
    this->decode_dn_op(curr_data, ins);
  }

  // Copy and return
  return ins;
}
