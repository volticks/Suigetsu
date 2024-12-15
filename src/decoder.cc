#include "decoder.h"
#include "instruction.h"
#include <cstring>
#include <iostream>
#include <ostream>

Instruction Decoder::decode_inst(const inst_data *curr_data) {

  std::cout << "Decoder::decode_inst opcode data: 0x" << std::hex
            << (int)*curr_data << std::endl;
  Instruction ins;
  // TODO: Actually decode stuff
  ins.op = InsnType::NOP;
  ins.sz = 1;
  memset(&ins.args, 0, sizeof(ins.args));
  // Copy and return
  return ins;
}
