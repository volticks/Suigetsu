#include "emulator.h"
#include "registers.h"
#include <iostream>
#include <ostream>

bool Emulator::emu_loop(const Instructions &insns) {

  const Instructions::const_iterator begin = insns.begin();
  const Instructions::const_iterator end = insns.end();
  Instructions::const_iterator it = begin;
  bool ret = true;

  Instruction curr_ins;
  while (it < end) {
    decoder.decode_inst(it.base(), end.base(), curr_ins);

    // std::cout << "Emulator::emu_loop decoded, sz: " << (int)curr_ins.sz
    //           << " OP (enum): " << std::hex << (int)curr_ins.op << std::endl;
    //  Instruction decoding would go OOB
    if (curr_ins.sz == 0) {
      std::cerr << "Emulator::emu_loop decoding went OOB OR failed, exiting."
                << std::endl;
      break;
    }
    // Decoding failed due to invalid ins stop execution - disabled for now to
    // enable better deocing testing.
    if (curr_ins.op == NONE) {
      std::cerr << "Emulator::emu_loop invalid instruction, exiting."
                << std::endl;
      ret = false;
      break;
    }

    execute_insn(curr_ins);

    // TODO: Maybe combine these 2 together, dunno how much sense it makes to
    // have pc and iter be 2 seperate things
    registers.set_pc(registers.get_pc() + curr_ins.sz);
    // std::cout << "Emulator::emu_loop, new PC: " << registers.get_pc()
    //           << std::endl;
    it += curr_ins.sz;
  }

  return true;
}

bool Emulator::execute_insn(const Instruction &insn) {
  // TODO: actual stuff here
  // std::cout << "Emulator::execute_insn doing stuff..." << std::endl;
  return true;
}
