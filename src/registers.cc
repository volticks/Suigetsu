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
