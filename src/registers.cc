#include "registers.h"
#include "emulator.h"
#include "instruction.h"
// Default, all registers 0

Reg::Reg() : registers{0} {}

reg_type Reg::get(ArgKind reg, bool logging) {
  if (reg > reg_usable || reg <= ArgKind::NONE)
    throw RegisterException("Reg::get OOB");

  if (is_mem_reg(reg)) {
    // Return/read the register equivalent to we can do stuf with its addresl
    // val
    reg = (ArgKind)((reg_type)reg - (reg_type)mem_reg_off);
  }

  reg_type val = registers[static_cast<uint32_t>(reg)];

  if (reg == ArgKind::PSW) {
    val &= 0xffff;
  }

  if (logging)
    std::cout << "Reg::get " << arg_kind_to_str(reg) << " has " << std::hex
              << val << std::endl;
  return val;
}
// Returns false for OOB accesses, true default
bool Reg::set(ArgKind reg, reg_type val) {
  if (reg > reg_usable || reg <= ArgKind::NONE)
    throw RegisterException("Reg::set OOB");

  // PSW is meant to be 16bits so we just ignore the top 16.
  if (reg == ArgKind::PSW) {
    val &= 0xffff;
  }

  std::cout << "Reg::set " << arg_kind_to_str(reg) << " --> " << std::hex << val
            << std::endl;

  registers[static_cast<uint32_t>(reg)] = val;
  return true;
}

reg_type Reg::get_pc() { return get(ArgKind::PC); }
void Reg::set_pc(reg_type pc) { set(ArgKind::PC, pc); }

void Reg::dump_regs() {
  printf("Reg::dump_regs, dumping all registers\n");
  uint32_t start = static_cast<uint32_t>(ArgKind::NONE) + 1;
  uint32_t end = static_cast<uint32_t>(reg_usable);
  for (; start < end; start++) {
    printf("%s\t:%08x\n", arg_kind_to_str((ArgKind)start),
           this->get((ArgKind)start, false));
  }
}

// const char *reg_to_str(ArgKind reg) {
//   switch (reg) {
//   case D0:
//     return "D0";
//   case D1:
//     return "D1";
//   case D2:
//     return "D2";
//   case D3:
//     return "D3";
//   case A0:
//     return "A0";
//   case A1:
//     return "A1";
//   case A2:
//     return "A2";
//   case A3:
//     return "A3";
//   case SP:
//     return "SP";
//   case PC:
//     return "PC";
//   case MDR:
//     return "MDR";
//   case PSW:
//     return "PSW";
//   case LIR:
//     return "LIR";
//   case LAR:
//     return "LAR";
//   default:
//     return "UNKNOWN";
//   }
// }
