#include "registers.h"
// Default, all registers 0

Reg::Reg() : registers{0} {}

reg_type Reg::get(RegTypes reg) {
  if (reg > reg_usable || reg < 0)
    throw RegisterException("Reg::get OOB");
  return registers[reg];
}
// Returns false for OOB accesses, true default
bool Reg::set(RegTypes reg, reg_type val) {
  if (reg > reg_usable || reg < 0)
    throw RegisterException("Reg::set OOB");
  registers[reg] = val;
  return true;
}

reg_type Reg::get_pc() { return get(PC); }
void Reg::set_pc(reg_type pc) { set(PC, pc); }

const char *reg_to_str(RegTypes reg) {
  switch (reg) {
  case D0:
    return "D0";
  case D1:
    return "D1";
  case D2:
    return "D2";
  case D3:
    return "D3";
  case A0:
    return "A0";
  case A1:
    return "A1";
  case A2:
    return "A2";
  case A3:
    return "A3";
  case SP:
    return "SP";
  case PC:
    return "PC";
  case MDR:
    return "MDR";
  case PSW:
    return "PSW";
  case LIR:
    return "LIR";
  case LAR:
    return "LAR";
  default:
    return "UNKNOWN";
  }
}
