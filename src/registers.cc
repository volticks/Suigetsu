#include "registers.h"
// Default, all registers 0

Reg::Reg() {}

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
