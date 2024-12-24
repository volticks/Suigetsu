#include "emulator.h"
#include "registers.h"
#include <iostream>
#include <ostream>

bool Emulator::emu_loop(const Instructions &insns) {

  const Instructions::const_iterator begin = insns.begin();
  const Instructions::const_iterator end = insns.end();
  Instructions::const_iterator it = begin;

  while (it < end) {
    // TODO: actually do stuff here
    Instruction ins = decoder.decode_inst(it.base(), end.base());

    // Instruction decoding would go OOB, or decoding failed stop execution
    if (ins.sz == 0)
      break;

    execute_insn(ins);

    // TODO: Maybe combine these 2 together, dunno how much sense it makes to
    // have pc and iter be 2 seperate things
    registers.set_pc(registers.get_pc() + ins.sz);
    std::cout << "Emulator::emu_loop, new PC: " << registers.get_pc()
              << std::endl;
    it += ins.sz;
  }

  return true;
}

bool Emulator::execute_insn(const Instruction &insn) {
  // TODO: actual stuff here
  std::cout << "Emulator::execute_insn doing stuff..." << std::endl;
  return true;
}
